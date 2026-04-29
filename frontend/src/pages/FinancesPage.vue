<template>
  <div>
    <h2 style="margin-bottom:1.5rem;font-size:1.4rem">💰 Finances</h2>

    <div class="kpi-grid" v-if="resume.length">
      <div class="kpi-card">
        <div class="kpi-value" style="color:var(--success)">
          {{ formatM(totalRecettes) }}
        </div>
        <div class="kpi-label">Total recettes (DA)</div>
      </div>
      <div class="kpi-card">
        <div class="kpi-value" style="color:var(--danger)">
          {{ formatM(totalDepenses) }}
        </div>
        <div class="kpi-label">Total dépenses (DA)</div>
      </div>
      <div class="kpi-card">
        <div class="kpi-value" :style="{ color: solde >= 0 ? 'var(--success)' : 'var(--danger)' }">
          {{ formatM(solde) }}
        </div>
        <div class="kpi-label">Solde</div>
      </div>
    </div>

    <div class="charts-grid">
      <div class="card">
        <div class="card-title">Recettes vs Dépenses par année</div>
        <Bar v-if="annualChart.labels.length" :data="annualChart" :options="barOpts" />
      </div>
      <div class="card">
        <div class="card-title">Dépenses par poste budgétaire</div>
        <Doughnut v-if="posteChart.labels.length" :data="posteChart" :options="{ responsive: true }" />
      </div>
    </div>

    <div class="card">
      <div class="filters-bar">
        <div class="form-group">
          <label>Année</label>
          <select v-model="filters.annee">
            <option value="">Toutes</option>
            <option v-for="a in annees" :key="a" :value="a">{{ a }}</option>
          </select>
        </div>
        <div class="form-group">
          <label>Type</label>
          <select v-model="filters.type">
            <option value="">Tous</option>
            <option value="Dépense">Dépense</option>
            <option value="Recette">Recette</option>
          </select>
        </div>
        <button class="btn btn-primary" @click="loadTable">🔍 Filtrer</button>
      </div>

      <table class="data-table">
        <thead>
          <tr><th>Année</th><th>Type</th><th>Poste</th><th>Montant (DA)</th><th>Département</th></tr>
        </thead>
        <tbody>
          <tr v-for="f in finances" :key="f.finance_id">
            <td>{{ f.annee }}</td>
            <td><span :class="['badge', f.type === 'Recette' ? 'badge-success' : 'badge-danger']">{{ f.type }}</span></td>
            <td>{{ f.poste_budgetaire }}</td>
            <td>{{ Number(f.montant).toLocaleString('fr-DZ') }}</td>
            <td>{{ f.departement }}</td>
          </tr>
        </tbody>
      </table>
    </div>
  </div>
</template>

<script setup>
import { ref, computed, onMounted } from 'vue'
import { Bar, Doughnut } from 'vue-chartjs'
import {
  Chart as ChartJS, CategoryScale, LinearScale, BarElement,
  Title, Tooltip, Legend, ArcElement,
} from 'chart.js'
import api from '../api'

ChartJS.register(CategoryScale, LinearScale, BarElement, Title, Tooltip, Legend, ArcElement)

const finances = ref([])
const resume = ref([])
const posteData = ref([])
const annees = [2020, 2021, 2022, 2023, 2024]
const filters = ref({ annee: '', type: '' })

const barOpts = { responsive: true, plugins: { legend: { position: 'bottom' } } }

const totalRecettes = computed(() => resume.value.reduce((s, r) => s + parseFloat(r.total_recettes || 0), 0))
const totalDepenses = computed(() => resume.value.reduce((s, r) => s + parseFloat(r.total_depenses || 0), 0))
const solde = computed(() => totalRecettes.value - totalDepenses.value)

function formatM(v) {
  if (v >= 1e6) return (v / 1e6).toFixed(1) + 'M'
  if (v >= 1e3) return (v / 1e3).toFixed(0) + 'K'
  return v.toFixed(0)
}

const annualChart = computed(() => {
  const anneeMap = {}
  for (const r of resume.value) {
    if (!anneeMap[r.annee]) anneeMap[r.annee] = { rec: 0, dep: 0 }
    anneeMap[r.annee].rec += parseFloat(r.total_recettes || 0)
    anneeMap[r.annee].dep += parseFloat(r.total_depenses || 0)
  }
  const years = Object.keys(anneeMap).sort()
  return {
    labels: years,
    datasets: [
      { label: 'Recettes', data: years.map((y) => anneeMap[y].rec), backgroundColor: '#057a55', borderRadius: 4 },
      { label: 'Dépenses', data: years.map((y) => anneeMap[y].dep), backgroundColor: '#c81e1e', borderRadius: 4 },
    ],
  }
})

const posteChart = computed(() => {
  const depenses = posteData.value.filter((p) => p.type === 'Dépense')
  return {
    labels: depenses.map((p) => p.poste),
    datasets: [{ data: depenses.map((p) => parseFloat(p.total)), backgroundColor: COLORS }],
  }
})

const COLORS = ['#1a56db','#7e3af2','#057a55','#c27803','#c81e1e','#0694a2','#3f83f8','#f05252','#31c48d','#e3a008']

async function loadTable() {
  const params = { limit: 100 }
  if (filters.value.annee) params.annee = filters.value.annee
  if (filters.value.type) params.type = filters.value.type
  const res = await api.get('/finances/', { params })
  finances.value = res.data
}

onMounted(async () => {
  const [resRes, posteRes] = await Promise.all([
    api.get('/finances/resume'),
    api.get('/finances/par-poste'),
  ])
  resume.value = resRes.data
  posteData.value = posteRes.data
  await loadTable()
})
</script>
