import { createRouter, createWebHistory } from 'vue-router'
import { useAuthStore } from '../store/auth'

const routes = [
  { path: '/login', name: 'Login', component: () => import('../pages/LoginPage.vue'), meta: { public: true } },
  {
    path: '/',
    component: () => import('../pages/LayoutPage.vue'),
    meta: { requiresAuth: true },
    children: [
      { path: '', name: 'Dashboard', component: () => import('../pages/DashboardPage.vue') },
      { path: 'etudiants', name: 'Etudiants', component: () => import('../pages/EtudiantsPage.vue') },
      { path: 'enseignants', name: 'Enseignants', component: () => import('../pages/EnseignantsPage.vue') },
      { path: 'finances', name: 'Finances', component: () => import('../pages/FinancesPage.vue') },
      { path: 'recherche', name: 'Recherche', component: () => import('../pages/RecherchePage.vue') },
    ],
  },
]

const router = createRouter({
  history: createWebHistory(),
  routes,
})

router.beforeEach((to) => {
  const auth = useAuthStore()
  if (!to.meta.public && !auth.isAuthenticated) {
    return { name: 'Login' }
  }
  if (to.name === 'Login' && auth.isAuthenticated) {
    return { name: 'Dashboard' }
  }
})

export default router
