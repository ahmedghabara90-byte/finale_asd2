<template>
  <div>
    <h2 style="margin-bottom:1.5rem;font-size:1.4rem">📊 Tableau de bord principal</h2>

    <!-- KPI Cards -->
    <div class="kpi-grid">
      <div class="kpi-card">
        <div class="kpi-value">{{ kpi.nb_etudiants ?? '—' }}</div>
        <div class="kpi-label">Étudiants</div>
      </div>
      <div class="kpi-card">
        <div class="kpi-value">{{ kpi.nb_enseignants ?? '—' }}</div>
        <div class="kpi-label">Enseignants</div>
      </div>
      <div class="kpi-card">
        <div class="kpi-value">{{ kpi.taux_reussite_moyen_pct ?? '—' }}%</div>
        <div class="kpi-label">Taux de réussite moyen</div>
      </div>
      <div class="kpi-card">
        <div class="kpi-value">{{ kpi.taux_abandon_moyen_pct ?? '—' }}%</div>
        <div class="kpi-label">Taux d'abandon moyen</div>
      </div>
      <div class="kpi-card">
        <div class="kpi-value">{{ kpi.nb_projets_recherche ?? '—' }}</div>
        <div class="kpi-label">Projets de recherche</div>
      </div>
      <div class="kpi-card">
        <div class="kpi-value">{{ kpi.total_publications ?? '—' }}</div>
        <div class="kpi-label">Publications scientifiques</div>
      </div>
    </div>

    <!-- Charts -->
    <div class="charts-grid">
      <!-- Inscrits par filière -->
      <div class="card">
        <div class="card-title">Inscrits par filière (dernière année)</div>
        <Bar v-if="inscritChart.labels.length" :data="inscritChart" :options="chartOptions" />
        <p v-else style="color:var(--text-muted);font-size:.9rem">Chargement...</p>
      </div>
      <!-- Taux de réussite -->
      <div class="card">
        <div class="card-title">Taux de réussite par filière</div>
        <Bar v-if="reussiteChart.labels.length" :data="reussiteChart" :options="chartOptions" />
        <p v-else style="color:var(--text-muted);font-size:.9rem">Chargement...</p>
      </div>
      <!-- Finances -->
      <div class="card">
        <div class="card-title">Recettes vs Dépenses globales</div>
        <Doughnut v-if="financeChart.labels.length" :data="financeChart" :options="{ responsive: true }" />
        <p v-else style="color:var(--text-muted);font-size:.9rem">Chargement...</p>
      </div>
      <!-- Publications -->
      <div class="card">
        <div class="card-title">Publications par département</div>
        <Bar v-if="pubChart.labels.length" :data="pubChart" :options="chartOptions" />
        <p v-else style="color:var(--text-muted);font-size:.9rem">Chargement...</p>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { Bar, Doughnut } from 'vue-chartjs'
import {
  Chart as ChartJS, CategoryScale, LinearScale, BarElement,
  Title, Tooltip, Legend, ArcElement,
} from 'chart.js'
import api from '../api'

ChartJS.register(CategoryScale, LinearScale, BarElement, Title, Tooltip, Legend, ArcElement)

const kpi = ref({})

const chartOptions = { responsive: true, plugins: { legend: { display: false } } }

const inscritChart  = ref({ labels: [], datasets: [] })
const reussiteChart = ref({ labels: [], datasets: [] })
const financeChart  = ref({ labels: [], datasets: [] })
const pubChart      = ref({ labels: [], datasets: [] })

function buildBarChart(labels, data, color = '#1a56db') {
  return {
    labels,
    datasets: [{ data, backgroundColor: color, borderRadius: 4 }],
  }
}

onMounted(async () => {
  try {
    const [resumeRes, inscritsRes, reussiteRes, financeRes, pubRes] = await Promise.all([
      api.get('/kpis/resume'),
      api.get('/kpis/inscrits-par-filiere'),
      api.get('/kpis/taux-reussite'),
      api.get('/kpis/budget-depenses'),
      api.get('/kpis/publications'),
    ])

    kpi.value = resumeRes.data

    // Inscrits par filière (dernière année disponible)
    const inscritsData = inscritsRes.data
    const maxAnnee = Math.max(...inscritsData.map((r) => r.annee))
    const inscritsFiltered = inscritsData.filter((r) => r.annee === maxAnnee)
    inscritChart.value = buildBarChart(
      inscritsFiltered.map((r) => r.nom_filiere),
      inscritsFiltered.map((r) => r.nb_inscrits),
      '#1a56db',
    )

    // Taux de réussite (dernière année, sem 1)
    const rData = reussiteRes.data
    const maxAr = Math.max(...rData.map((r) => r.annee))
    const rFiltered = rData.filter((r) => r.annee === maxAr && r.semestre === 1)
    reussiteChart.value = buildBarChart(
      rFiltered.map((r) => r.nom_filiere),
      rFiltered.map((r) => parseFloat(r.taux_reussite_pct)),
      '#057a55',
    )

    // Finances
    const fData = financeRes.data
    const totalRecettes = fData.filter((r) => r.type === 'Recette').reduce((s, r) => s + r.total_recettes, 0) ||
      fData.reduce((s, r) => s + parseFloat(r.total_recettes || 0), 0)
    const totalDepenses = fData.reduce((s, r) => s + parseFloat(r.total_depenses || 0), 0)
    financeChart.value = {
      labels: ['Recettes', 'Dépenses'],
      datasets: [{
        data: [totalRecettes, totalDepenses],
        backgroundColor: ['#057a55', '#c81e1e'],
      }],
    }

    // Publications
    const pData = pubRes.data.slice(0, 8)
    pubChart.value = buildBarChart(
      pData.map((r) => r.departement),
      pData.map((r) => r.total_publications),
      '#7e3af2',
    )
  } catch (e) {
    console.error('Erreur chargement dashboard:', e)
  }
})
</script>
