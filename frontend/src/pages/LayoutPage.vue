<template>
  <div class="layout">
    <aside class="sidebar">
      <div class="logo">🎓 UPES BI</div>
      <nav>
        <router-link to="/"           exact-active-class="active">📊 Dashboard</router-link>
        <router-link to="/etudiants"  active-class="active">🎓 Étudiants</router-link>
        <router-link to="/enseignants" active-class="active">👨‍🏫 Enseignants</router-link>
        <router-link to="/finances"   active-class="active">💰 Finances</router-link>
        <router-link to="/recherche"  active-class="active">🔬 Recherche</router-link>
      </nav>
      <div style="margin-top:auto;padding:1rem 1.5rem;font-size:.8rem;color:rgba(255,255,255,.6)">
        Connecté en tant que <strong style="color:#fff">{{ auth.username }}</strong>
        <br /><span style="text-transform:capitalize">{{ auth.role }}</span>
        <br /><br />
        <button class="btn btn-secondary" style="font-size:.8rem;padding:.4rem .8rem" @click="logout">
          Déconnexion
        </button>
      </div>
    </aside>
    <main class="main-content">
      <router-view />
    </main>
  </div>
</template>

<script setup>
import { useAuthStore } from '../store/auth'
import { useRouter } from 'vue-router'

const auth = useAuthStore()
const router = useRouter()

function logout() {
  auth.logout()
  router.push({ name: 'Login' })
}
</script>
