import { createRouter, createWebHistory, RouteRecordRaw } from 'vue-router';

const routes: RouteRecordRaw[] = [
  {
    path: '/',
    name: 'Begin',
    component: () => import('../views/BeginView.vue'),
  },
  {
    path: '/login',
    name: 'Login',
    component: () => import('../views/LoginView.vue'),
  },
  {
    path: '/register',
    name: 'Register',
    component: () => import('../views/RegisterView.vue'),
  },
  {
    path: '/reset-password',
    name: 'ResetPassword',
    component: () => import('../views/ResetPasswordView.vue'),
  },
  {
    path: '/home',
    name: 'Home',
    component: () => import('../views/HomeView.vue'),
    meta: { requiresAuth: true },
  },
  {
    path: '/menu',
    name: 'Menu',
    component: () => import('../views/MenuView.vue'),
    meta: { requiresAuth: true },
  },
  {
    path: '/order',
    name: 'Order',
    component: () => import('../views/OrderView.vue'),
    meta: { requiresAuth: true },
  },
  {
    path: '/merchant',
    name: 'Merchant',
    component: () => import('../views/MerchantView.vue'),
    meta: { requiresAuth: true },
  },
  {
    path: '/admin',
    name: 'Admin',
    component: () => import('../views/AdminView.vue'),
    meta: { requiresAuth: true, requiresAdmin: true },
  },
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

// 路由守卫
router.beforeEach((to, from, next) => {
  const token = localStorage.getItem('token');
  const isAdmin = localStorage.getItem('isAdmin') === 'true';

  if (to.meta.requiresAuth && !token) {
    next('/login');
  } else if (to.meta.requiresAdmin && !isAdmin) {
    next('/home');
  } else {
    next();
  }
});

export default router; 