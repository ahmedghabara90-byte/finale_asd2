<template>
  <div>
    <h2 style="margin-bottom:1.5rem;font-size:1.4rem">🔬 Recherche Scientifique</h2>

    <div class="kpi-grid" v-if="stats.length">
      <div class="kpi-card">
        <div class="kpi-value">{{ totalProjets }}</div>
        <div class="kpi-label">Projets</div>
      </div>
      <div class="kpi-card">
        <div class="kpi-value">{{ totalPub }}</div>
        <div class="kpi-label">Publications</div>
      </div>
      <div class="kpi-card">
        <div class="kpi-value">{{ formatM(totalBudget) }} DA</div>
        <div class="kpi-label">Budget total</div>
      </div>
    </div>

    <div class="charts-grid">
      <div class="card">
        <div class="card-title">Publications par département</div>
        <Bar v-if="pubChart.labels.length" :data="pubChart" :options="barOpts" />
      </div>
      <div class="card">
        <div class="card-title">Budget recherche par département</div>
        <Bar v-if="budgetChart.labels.length" :data="budgetChart" :options="barOpts" />
      </div>
    </div>

    <div class="card">
      <div class="filters-bar">
        <div class="form-group">
          <label>Statut</label>
          <select v-model="filters.statut">
            <option value="">Tous</option>
            <option value="En cours">En cours</option>
            <option value="Terminé">Terminé</option>
            <option value="Suspendu">Suspendu</option>
          </select>
        </div>
        <div class="form-group">
          <label>Département</label>
          <select v-model="filters.departement">
            <option value="">Tous</option>
            <option v-for="d in departements" :key="d" :value="d">{{ d }}</option>
          </select>
        </div>
        <button class="btn btn-primary" @click="loadTable">🔍 Filtrer</button>
      </div>

      <table class="data-table">
        <thead>
          <tr><th>ID</th><th>Département</th><th>Thématique</th><th>Budget (DA)</th><th>Publications</th><th>Statut</th></tr>
        </thead>
        <tbody>
          <tr v-for="p in projets" :key="p.projet_id">
            <td>{{ p.projet_id }}</td>
            <td>{{ p.departement }}</td>
            <td>{{ p.thematique }}</td>
            <td>{{ Number(p.budget).toLocaleString('fr-DZ') }}</td>
            <td>{{ p.publications }}</td>
            <td><span :class="['badge', statutClass(p.statut)]">{{ p.statut }}</span></td>
          </tr>
        </tbody>
      </table>
    </div>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import { Bar } from 'vue-chartjs'
import { Chart as ChartJS, CategoryScale, LinearScale, BarElement, Title, Tooltip, Legend } from 'chart.js'
import api from '../api'

ChartJS.register(CategoryScale, LinearScale, BarElement, Title, Tooltip, Legend)

const projets = ref([])
const stats = ref([])
const departements = ref([])
const filters = ref({ statut: '', departement: '' })
const barOpts = { responsive: true, plugins: { legend: { display: false } } }

const totalProjets = computed(() => stats.value.reduce((s, r) => s + r.nb_projets, 0))
const totalPub = computed(() => stats.value.reduce((s, r) => s + r.total_publications, 0))
const totalBudget = computed(() => stats.value.reduce((s, r) => s + r.budget_total, 0))

function formatM(v) {
  if (v >= 1e6) return (v / 1e6).toFixed(1) + 'M'
  return (v / 1e3).toFixed(0) + 'K'
}

function statutClass(s) {
  if (s === 'Terminé') return 'badge-success'
  if (s === 'Suspendu') return 'badge-warning'
  return 'badge-info'
}

const pubChart = computed(() => ({
  labels: stats.value.map((r) => r.departement),
  datasets: [{ data: stats.value.map((r) => r.total_publications), backgroundColor: '#7e3af2', borderRadius: 4 }],
}))

const budgetChart = computed(() => ({
  labels: stats.value.map((r) => r.departement),
  datasets: [{ data: stats.value.map((r) => r.budget_total / 1e6), backgroundColor: '#1a56db', borderRadius: 4 }],
}))

async function loadTable() {
  const params = { limit: 100 }
  if (filters.value.statut) params.statut = filters.value.statut
  if (filters.value.departement) params.departement = filters.value.departement
  const res = await api.get('/recherche/', { params })
  projets.value = res.data
}

onMounted(async () => {
  const [statsRes, listRes] = await Promise.all([
    api.get('/recherche/stats'),
    api.get('/recherche/', { params: { limit: 100 } }),
  ])
  stats.value = statsRes.data
  projets.value = listRes.data
  departements.value = statsRes.data.map((r) => r.departement)
})
</script>
