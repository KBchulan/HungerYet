<template>
  <div class="login-container">
    <el-card class="login-card">
      <template #header>
        <h2>登录</h2>
      </template>
      
      <el-form
        ref="loginFormRef"
        :model="loginForm"
        :rules="rules"
        label-width="80px"
        @submit.prevent="handleLogin"
      >
        <el-form-item label="邮箱" prop="email">
          <el-input
            v-model="loginForm.email"
            type="email"
            placeholder="请输入邮箱"
            clearable
          />
        </el-form-item>

        <el-form-item label="密码" prop="password">
          <el-input
            v-model="loginForm.password"
            type="password"
            placeholder="请输入密码"
            show-password
            clearable
          />
        </el-form-item>

        <el-form-item>
          <el-button type="primary" @click="handleLogin" :loading="loading">
            登录
          </el-button>
          <el-button @click="goToRegister">注册</el-button>
        </el-form-item>

        <div class="login-links">
          <el-link type="primary" @click="goToResetPassword">忘记密码？</el-link>
        </div>
      </el-form>
    </el-card>
  </div>
</template>

<script lang="ts" setup>
import { ref, reactive } from 'vue';
import { useRouter } from 'vue-router';
import { ElMessage } from 'element-plus';
import type { FormRules, FormInstance } from 'element-plus';
import { useUserStore } from '../stores/userStore';

const router = useRouter();
const userStore = useUserStore();
const loading = ref(false);
const loginFormRef = ref<FormInstance>();

const loginForm = reactive({
  email: '',
  password: '',
});

const rules = reactive<FormRules>({
  email: [
    { required: true, message: '请输入邮箱', trigger: 'blur' },
    { type: 'email', message: '请输入正确的邮箱格式', trigger: 'blur' },
  ],
  password: [
    { required: true, message: '请输入密码', trigger: 'blur' },
    { min: 6, message: '密码长度不能小于6位', trigger: 'blur' },
  ],
});

const handleLogin = async () => {
  if (!loginFormRef.value) return;
  
  await loginFormRef.value.validate(async (valid, fields) => {
    if (valid) {
      loading.value = true;
      try {
        const success = await userStore.login(loginForm.email, loginForm.password);
        if (success) {
          ElMessage.success('登录成功');
          router.push('/home');
        } else {
          ElMessage.error('登录失败，请检查邮箱和密码');
        }
      } catch (error) {
        ElMessage.error('登录过程中发生错误');
      } finally {
        loading.value = false;
      }
    } else {
      console.log('验证失败:', fields);
    }
  });
};

const goToRegister = () => {
  router.push('/register');
};

const goToResetPassword = () => {
  router.push('/reset-password');
};
</script>

<style lang="scss" scoped>
.login-container {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  background-color: #f5f7fa;

  .login-card {
    width: 400px;
    border-radius: 8px;
    box-shadow: 0 2px 12px 0 rgba(0, 0, 0, 0.1);

    :deep(.el-card__header) {
      text-align: center;
      padding: 20px;
      border-bottom: 1px solid #ebeef5;
      
      h2 {
        margin: 0;
        color: #303133;
      }
    }
  }

  .login-links {
    text-align: right;
    margin-top: 10px;
  }

  :deep(.el-form-item__content) {
    display: flex;
    justify-content: center;
    gap: 10px;
  }
}
</style> 