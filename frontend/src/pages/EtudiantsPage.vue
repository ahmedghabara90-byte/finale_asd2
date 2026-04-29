<template>
  <div>
    <h2 style="margin-bottom:1.5rem;font-size:1.4rem">🎓 Étudiants</h2>

    <!-- Statistiques -->
    <div class="kpi-grid" v-if="stats">
      <div class="kpi-card">
        <div class="kpi-value">{{ stats.total }}</div>
        <div class="kpi-label">Total étudiants</div>
      </div>
      <div class="kpi-card" v-for="s in stats.par_statut" :key="s.statut">
        <div class="kpi-value">{{ s.nb }}</div>
        <div class="kpi-label">{{ s.statut }}</div>
      </div>
    </div>

    <!-- Filtres -->
    <div class="card">
      <div class="filters-bar">
        <div class="form-group">
          <label>Filière</label>
          <select v-model="filters.filiere">
            <option value="">Toutes</option>
            <option v-for="f in filieres" :key="f" :value="f">{{ f }}</option>
          </select>
        </div>
        <div class="form-group">
          <label>Statut</label>
          <select v-model="filters.statut">
            <option value="">Tous</option>
            <option value="Inscrit">Inscrit</option>
            <option value="Diplômé">Diplômé</option>
            <option value="Abandonné">Abandonné</option>
          </select>
        </div>
        <div class="form-group">
          <label>Année inscription</label>
          <select v-model="filters.annee">
            <option value="">Toutes</option>
            <option v-for="a in annees" :key="a" :value="a">{{ a }}</option>
          </select>
        </div>
        <button class="btn btn-primary" @click="loadData">🔍 Filtrer</button>
        <button class="btn btn-secondary" @click="resetFilters">✕ Réinitialiser</button>
      </div>

      <table class="data-table">
        <thead>
          <tr>
            <th>ID</th><th>Nom</th><th>Filière</th><th>Année</th><th>Statut</th><th>Email</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="e in etudiants" :key="e.etudiant_id">
            <td>{{ e.etudiant_id }}</td>
            <td>{{ e.nom }}</td>
            <td>{{ e.filiere }}</td>
            <td>{{ e.annee_inscription }}</td>
            <td>
              <span :class="['badge', badgeClass(e.statut)]">{{ e.statut }}</span>
            </td>
            <td>{{ e.email }}</td>
          </tr>
        </tbody>
      </table>

      <div class="pagination">
        <button @click="prevPage" :disabled="page === 0">‹ Précédent</button>
        <button class="active">{{ page + 1 }}</button>
        <button @click="nextPage" :disabled="etudiants.length < pageSize">Suivant ›</button>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import api from '../api'

const etudiants = ref([])
const stats = ref(null)
const filieres = ref([])
const annees = [2020, 2021, 2022, 2023, 2024]
const page = ref(0)
const pageSize = 50
const filters = ref({ filiere: '', statut: '', annee: '' })

function badgeClass(statut) {
  if (statut === 'Inscrit') return 'badge-info'
  if (statut === 'Diplômé') return 'badge-success'
  if (statut === 'Abandonné') return 'badge-danger'
  return 'badge-info'
}

async function loadData() {
  const params = { limit: pageSize, offset: page.value * pageSize }
  if (filters.value.filiere) params.filiere = filters.value.filiere
  if (filters.value.statut) params.statut = filters.value.statut
  if (filters.value.annee) params.annee = filters.value.annee
  const res = await api.get('/etudiants/', { params })
  etudiants.value = res.data
}

function resetFilters() {
  filters.value = { filiere: '', statut: '', annee: '' }
  page.value = 0
  loadData()
}

function prevPage() { if (page.value > 0) { page.value--; loadData() } }
function nextPage() { page.value++; loadData() }

onMounted(async () => {
  const [statsRes, etudRes] = await Promise.all([
    api.get('/etudiants/stats'),
    api.get('/etudiants/', { params: { limit: pageSize, offset: 0 } }),
  ])
  stats.value = statsRes.data
  etudiants.value = etudRes.data
  filieres.value = statsRes.data.par_filiere.map((f) => f.filiere)
})
</script>
