<template>
  <div class="register-container">
    <div class="register-box">
      <h2>注册</h2>
      <form @submit.prevent="handleRegister">
        <div class="form-group">
          <label for="email">邮箱</label>
          <div class="input-group">
            <input
              type="email"
              id="email"
              v-model="form.email"
              required
              placeholder="请输入邮箱"
              :disabled="loading"
            >
            <button 
              type="button" 
              class="verify-code-button"
              @click="handleGetVerifyCode"
              :disabled="loading || cooldown > 0"
            >
              {{ cooldown > 0 ? `${cooldown}s` : '获取验证码' }}
            </button>
          </div>
        </div>
        <div class="form-group">
          <label for="code">验证码</label>
          <input
            type="text"
            id="code"
            v-model="form.code"
            required
            placeholder="请输入验证码"
            :disabled="loading"
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
            :disabled="loading"
          >
        </div>
        <div class="form-group">
          <label for="confirmPassword">确认密码</label>
          <input
            type="password"
            id="confirmPassword"
            v-model="form.confirmPassword"
            required
            placeholder="请再次输入密码"
            :disabled="loading"
          >
        </div>
        <div v-if="errorMessage" class="error-message">
          {{ errorMessage }}
        </div>
        <div class="form-actions">
          <button type="submit" class="register-button" :disabled="loading">
            {{ loading ? '注册中...' : '注册' }}
          </button>
          <router-link to="/login" class="login-link">已有账号？去登录</router-link>
        </div>
      </form>
    </div>
  </div>
</template>

<script lang="ts">
import { ref, defineComponent } from 'vue'
import { useRouter } from 'vue-router'
import api from '@/api'

export default defineComponent({
  name: 'Register',
  setup() {
    const router = useRouter()
    const form = ref({
      email: '',
      code: '',
      password: '',
      confirmPassword: ''
    })
    const loading = ref(false)
    const errorMessage = ref('')
    const cooldown = ref(0)

    const startCooldown = () => {
      cooldown.value = 60
      const timer = setInterval(() => {
        cooldown.value--
        if (cooldown.value <= 0) {
          clearInterval(timer)
        }
      }, 1000)
    }

    const handleGetVerifyCode = async () => {
      if (cooldown.value > 0) return

      try {
        if (!form.value.email) {
          errorMessage.value = '请输入邮箱'
          return
        }

        loading.value = true
        errorMessage.value = ''

        await api.getVerifyCode({ email: form.value.email })
        startCooldown()
      } catch (error) {
        console.error('获取验证码错误:', error)
        errorMessage.value = '获取验证码失败，请重试'
      } finally {
        loading.value = false
      }
    }

    const handleRegister = async () => {
      try {
        loading.value = true
        errorMessage.value = ''

        // 验证密码
        if (form.value.password !== form.value.confirmPassword) {
          errorMessage.value = '两次输入的密码不一致'
          return
        }

        // 验证密码格式
        const passwordRegex = /^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)[a-zA-Z\d]{8,}$/
        if (!passwordRegex.test(form.value.password)) {
          errorMessage.value = '密码必须包含大小写字母和数字，且长度至少为8位'
          return
        }

        const response = await api.register({
          email: form.value.email,
          password: form.value.password,
          code: form.value.code
        })

        if (response.error === 0) {
          // 注册成功，跳转到登录页
          router.push('/login')
        } else {
          errorMessage.value = response.message || '注册失败，请重试'
        }
      } catch (error) {
        console.error('注册错误:', error)
        errorMessage.value = '注册失败，请重试'
      } finally {
        loading.value = false
      }
    }

    return {
      form,
      loading,
      errorMessage,
      cooldown,
      handleGetVerifyCode,
      handleRegister
    }
  }
})
</script>

<style scoped>
.register-container {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  background-color: #f5f5f5;
}

.register-box {
  background: white;
  padding: 2rem;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
  width: 100%;
  max-width: 400px;
}

.register-box h2 {
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

.input-group {
  display: flex;
  gap: 1rem;
}

.input-group input {
  flex: 1;
}

.form-group input {
  width: 100%;
  padding: 0.75rem;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 1rem;
}

.verify-code-button {
  padding: 0 1rem;
  background-color: #4CAF50;
  color: white;
  border: none;
  border-radius: 4px;
  font-size: 0.9rem;
  white-space: nowrap;
}

.verify-code-button:disabled {
  background-color: #ccc;
  cursor: not-allowed;
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

.register-button {
  width: 100%;
  padding: 0.75rem;
  background-color: #4CAF50;
  color: white;
  border: none;
  border-radius: 4px;
  font-size: 1rem;
  cursor: pointer;
}

.register-button:disabled {
  background-color: #ccc;
  cursor: not-allowed;
}

.login-link {
  text-align: center;
  color: #666;
  text-decoration: none;
}
</style> 