<template>
  <div>
    <h2 style="margin-bottom:1.5rem;font-size:1.4rem">👨‍🏫 Enseignants</h2>

    <div class="kpi-grid" v-if="stats">
      <div class="kpi-card">
        <div class="kpi-value">{{ stats.total }}</div>
        <div class="kpi-label">Total enseignants</div>
      </div>
      <div class="kpi-card" v-for="g in stats.par_grade.slice(0,3)" :key="g.grade">
        <div class="kpi-value">{{ g.nb }}</div>
        <div class="kpi-label" style="font-size:.75rem">{{ g.grade }}</div>
      </div>
    </div>

    <div class="card">
      <div class="filters-bar">
        <div class="form-group">
          <label>Département</label>
          <select v-model="filters.departement">
            <option value="">Tous</option>
            <option v-for="d in departements" :key="d" :value="d">{{ d }}</option>
          </select>
        </div>
        <div class="form-group">
          <label>Grade</label>
          <select v-model="filters.grade">
            <option value="">Tous</option>
            <option v-for="g in grades" :key="g" :value="g">{{ g }}</option>
          </select>
        </div>
        <button class="btn btn-primary" @click="loadData">🔍 Filtrer</button>
        <button class="btn btn-secondary" @click="reset">✕ Réinitialiser</button>
      </div>

      <table class="data-table">
        <thead>
          <tr><th>ID</th><th>Nom</th><th>Grade</th><th>Département</th><th>Email</th></tr>
        </thead>
        <tbody>
          <tr v-for="e in enseignants" :key="e.enseignant_id">
            <td>{{ e.enseignant_id }}</td>
            <td>{{ e.nom }}</td>
            <td><span class="badge badge-info">{{ e.grade }}</span></td>
            <td>{{ e.departement }}</td>
            <td>{{ e.email }}</td>
          </tr>
        </tbody>
      </table>

      <div class="pagination">
        <button @click="prev" :disabled="page === 0">‹ Précédent</button>
        <button class="active">{{ page + 1 }}</button>
        <button @click="next" :disabled="enseignants.length < pageSize">Suivant ›</button>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import api from '../api'

const enseignants = ref([])
const stats = ref(null)
const departements = ref([])
const grades = ['Professeur', 'Maître de Conférences A', 'Maître de Conférences B', 'Maître Assistant A', 'Maître Assistant B']
const page = ref(0)
const pageSize = 50
const filters = ref({ departement: '', grade: '' })

async function loadData() {
  const params = { limit: pageSize, offset: page.value * pageSize }
  if (filters.value.departement) params.departement = filters.value.departement
  if (filters.value.grade) params.grade = filters.value.grade
  const res = await api.get('/enseignants/', { params })
  enseignants.value = res.data
}

function reset() { filters.value = { departement: '', grade: '' }; page.value = 0; loadData() }
function prev() { if (page.value > 0) { page.value--; loadData() } }
function next() { page.value++; loadData() }

onMounted(async () => {
  const [statsRes, listRes] = await Promise.all([
    api.get('/enseignants/stats'),
    api.get('/enseignants/', { params: { limit: pageSize } }),
  ])
  stats.value = statsRes.data
  enseignants.value = listRes.data
  departements.value = statsRes.data.par_departement.map((d) => d.departement)
})
</script>
