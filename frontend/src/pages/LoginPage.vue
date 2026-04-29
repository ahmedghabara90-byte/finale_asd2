<template>
  <div class="login-wrapper">
    <div class="login-box">
      <h1>🎓 UPES BI Portal</h1>
      <p style="text-align:center;color:var(--text-muted);margin-bottom:1.5rem;font-size:.9rem">
        Portail Business Intelligence
      </p>
      <form @submit.prevent="handleLogin">
        <div class="form-group">
          <label>Identifiant</label>
          <input v-model="form.username" type="text" placeholder="admin / enseignant / etudiant" required />
        </div>
        <div class="form-group">
          <label>Mot de passe</label>
          <input v-model="form.password" type="password" placeholder="••••••••" required />
        </div>
        <div v-if="error" class="alert alert-error">{{ error }}</div>
        <button class="btn btn-primary" style="width:100%;justify-content:center" type="submit" :disabled="loading">
          {{ loading ? 'Connexion...' : 'Se connecter' }}
        </button>
      </form>
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import { useRouter } from 'vue-router'
import { useAuthStore } from '../store/auth'

const auth = useAuthStore()
const router = useRouter()
const form = ref({ username: '', password: '' })
const error = ref('')
const loading = ref(false)

async function handleLogin() {
  error.value = ''
  loading.value = true
  try {
    await auth.login(form.value.username, form.value.password)
    router.push({ name: 'Dashboard' })
  } catch (e) {
    error.value = e.response?.data?.detail || 'Identifiant ou mot de passe incorrect'
  } finally {
    loading.value = false
  }
}
</script>
