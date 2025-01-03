<template>
  <div class="login-container">
    <div class="login-box">
      <h2>登录</h2>
      <form @submit.prevent="handleLogin">
        <div class="form-group">
          <label for="email">邮箱</label>
          <input
            type="email"
            id="email"
            v-model="form.email"
            required
            placeholder="请输入邮箱"
          >
        </div>
        <div class="form-group">
          <label for="password">密码</label>
          <input
            type="password"
            id="password"
            v-model="form.password"
            required
            placeholder="请输入密码"
          >
        </div>
        <div v-if="errorMessage" class="error-message">
          {{ errorMessage }}
        </div>
        <div class="form-actions">
          <button type="submit" class="login-button" :disabled="loading">
            {{ loading ? '登录中...' : '登录' }}
          </button>
          <router-link to="/register" class="register-link">注册账号</router-link>
        </div>
      </form>
    </div>
  </div>
</template>

<script lang="ts">
import { ref, defineComponent } from 'vue'
import { useRouter } from 'vue-router'
import request from '@/utils/request'

interface LoginResponse {
  error: number
  uid: number
  token: string
  message?: string
}

export default defineComponent({
  name: 'Login',
  setup() {
    const router = useRouter()
    const form = ref({
      email: '',
      password: ''
    })
    const loading = ref(false)
    const errorMessage = ref('')

    const handleLogin = async () => {
      try {
        loading.value = true
        errorMessage.value = ''

        const response = await request.post<LoginResponse>('/api/user_login', {
          email: form.value.email,
          password: form.value.password,
          type: 1
        })

        console.log('登录响应:', response)

        if (response.error === 0) {
          // 登录成功
          const { uid, token } = response
          localStorage.setItem('token', token)
          localStorage.setItem('uid', uid.toString())
          router.push('/menu')
        } else if (response.error === 1009) {
          errorMessage.value = '密码错误'
        } else {
          errorMessage.value = response.message || '登录失败，请重试'
        }
      } catch (error) {
        console.error('登录错误:', error)
        errorMessage.value = '登录失败，请重试'
      } finally {
        loading.value = false
      }
    }

    return {
      form,
      loading,
      errorMessage,
      handleLogin
    }
  }
})
</script>

<style scoped>
.login-container {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  background-color: #f5f5f5;
}

.login-box {
  background: white;
  padding: 2rem;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
  width: 100%;
  max-width: 400px;
}

.login-box h2 {
  text-align: center;
  margin-bottom: 2rem;
  color: #333;
}

.form-group {
  margin-bottom: 1.5rem;
}

.form-group label {
  display: block;
  margin-bottom: 0.5rem;
  color: #666;
}

.form-group input {
  width: 100%;
  padding: 0.75rem;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 1rem;
}

.error-message {
  color: #ff5252;
  margin-bottom: 1rem;
  text-align: center;
}

.form-actions {
  display: flex;
  flex-direction: column;
  gap: 1rem;
  margin-top: 2rem;
}

.login-button {
  width: 100%;
  padding: 0.75rem;
  background-color: #4CAF50;
  color: white;
  border: none;
  border-radius: 4px;
  font-size: 1rem;
  cursor: pointer;
}

.login-button:disabled {
  background-color: #ccc;
  cursor: not-allowed;
}

.register-link {
  text-align: center;
  color: #666;
  text-decoration: none;
}
</style> 