import { defineStore } from 'pinia'
import axios from 'axios'

export const useAuthStore = defineStore('auth', {
  state: () => ({
    token: localStorage.getItem('token') || null,
    role: localStorage.getItem('role') || null,
    username: localStorage.getItem('username') || null,
  }),
  getters: {
    isAuthenticated: (state) => !!state.token,
    isAdmin: (state) => state.role === 'admin',
  },
  actions: {
    async login(username, password) {
      const form = new URLSearchParams()
      form.append('username', username)
      form.append('password', password)
      const { data } = await axios.post('/api/auth/token', form)
      this.token = data.access_token
      this.role = data.role
      this.username = username
      localStorage.setItem('token', data.access_token)
      localStorage.setItem('role', data.role)
      localStorage.setItem('username', username)
    },
    logout() {
      this.token = null
      this.role = null
      this.username = null
      localStorage.removeItem('token')
      localStorage.removeItem('role')
      localStorage.removeItem('username')
    },
  },
})
