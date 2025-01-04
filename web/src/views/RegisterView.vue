<template>
  <div class="register-container">
    <el-card class="register-card">
      <template #header>
        <h2>注册账号</h2>
      </template>
      
      <el-form
        ref="registerForm"
        :model="form"
        :rules="rules"
        label-width="80px"
        @submit.prevent="handleRegister"
      >
        <el-form-item label="用户名" prop="username">
          <el-input
            v-model="form.username"
            placeholder="请输入用户名"
          />
        </el-form-item>

        <el-form-item label="邮箱" prop="email">
          <el-input
            v-model="form.email"
            type="email"
            placeholder="请输入邮箱"
          >
            <template #append>
              <el-button
                :disabled="isGettingCode || countdown > 0"
                @click="getVerifyCode"
              >
                {{ countdown > 0 ? `${countdown}s` : '获取验证码' }}
              </el-button>
            </template>
          </el-input>
        </el-form-item>

        <el-form-item label="验证码" prop="verifyCode">
          <el-input
            v-model="form.verifyCode"
            placeholder="请输入验证码"
          />
        </el-form-item>

        <el-form-item label="密码" prop="password">
          <el-input
            v-model="form.password"
            type="password"
            placeholder="请输入密码"
            show-password
          />
        </el-form-item>

        <el-form-item label="确认密码" prop="confirmPassword">
          <el-input
            v-model="form.confirmPassword"
            type="password"
            placeholder="请再次输入密码"
            show-password
          />
        </el-form-item>

        <el-form-item>
          <el-button type="primary" @click="handleRegister" :loading="loading">
            注册
          </el-button>
          <el-button @click="goToLogin">返回登录</el-button>
        </el-form-item>
      </el-form>
    </el-card>
  </div>
</template>

<script lang="ts" setup>
import { ref, reactive } from 'vue';
import { useRouter } from 'vue-router';
import { ElMessage } from 'element-plus';
import HttpManager, { ReqId, Modules, ErrorCodes } from '../services/HttpManager';

const router = useRouter();
const loading = ref(false);
const isGettingCode = ref(false);
const countdown = ref(0);

const form = reactive({
  username: '',
  email: '',
  verifyCode: '',
  password: '',
  confirmPassword: '',
});

const validatePass = (rule: any, value: string, callback: any) => {
  if (value === '') {
    callback(new Error('请输入密码'));
  } else if (value.length < 6) {
    callback(new Error('密码长度不能小于6位'));
  } else {
    if (form.confirmPassword !== '') {
      if (form.confirmPassword !== value) {
        callback(new Error('两次输入密码不一致!'));
      }
    }
    callback();
  }
};

const validatePass2 = (rule: any, value: string, callback: any) => {
  if (value === '') {
    callback(new Error('请再次输入密码'));
  } else if (value !== form.password) {
    callback(new Error('两次输入密码不一致!'));
  } else {
    callback();
  }
};

const rules = {
  username: [
    { required: true, message: '请输入用户名', trigger: 'blur' },
    { min: 2, max: 20, message: '长度在 2 到 20 个字符', trigger: 'blur' }
  ],
  email: [
    { required: true, message: '请输入邮箱', trigger: 'blur' },
    { type: 'email', message: '请输入正确的邮箱格式', trigger: 'blur' }
  ],
  verifyCode: [
    { required: true, message: '请输入验证码', trigger: 'blur' },
    { len: 6, message: '验证码长度为6位', trigger: 'blur' }
  ],
  password: [
    { validator: validatePass, trigger: 'blur' }
  ],
  confirmPassword: [
    { validator: validatePass2, trigger: 'blur' }
  ]
};

const startCountdown = () => {
  countdown.value = 60;
  const timer = setInterval(() => {
    countdown.value--;
    if (countdown.value <= 0) {
      clearInterval(timer);
    }
  }, 1000);
};

const getVerifyCode = async () => {
  if (!form.email) {
    ElMessage.warning('请先输入邮箱');
    return;
  }
  
  isGettingCode.value = true;
  const httpManager = HttpManager.getInstance();
  
  return new Promise((resolve) => {
    const handleResponse = (id: ReqId, res: string, err: ErrorCodes, mod: Modules) => {
      if (err === ErrorCodes.SUCCESS) {
        const response = JSON.parse(res);
        if (response.code === 200) {
          ElMessage.success('验证码已发送到您的邮箱');
          startCountdown();
        } else {
          ElMessage.error(response.message || '获取验证码失败');
        }
      } else {
        ElMessage.error('网络错误，请稍后重试');
      }
      isGettingCode.value = false;
      httpManager.off('httpFinished', handleResponse);
      resolve(true);
    };

    httpManager.on('httpFinished', handleResponse);
    httpManager.postHttpReq('/get_varifycode', { email: form.email }, ReqId.REGISTER, Modules.REGISTERMOD);
  });
};

const handleRegister = async () => {
  loading.value = true;
  const httpManager = HttpManager.getInstance();
  
  return new Promise((resolve) => {
    const handleResponse = (id: ReqId, res: string, err: ErrorCodes, mod: Modules) => {
      if (err === ErrorCodes.SUCCESS) {
        const response = JSON.parse(res);
        if (response.code === 200) {
          ElMessage.success('注册成功');
          router.push('/login');
        } else {
          ElMessage.error(response.message || '注册失败');
        }
      } else {
        ElMessage.error('网络错误，请稍后重试');
      }
      loading.value = false;
      httpManager.off('httpFinished', handleResponse);
      resolve(true);
    };

    httpManager.on('httpFinished', handleResponse);
    httpManager.postHttpReq('/user_register', {
      username: form.username,
      email: form.email,
      password: form.password,
      verifyCode: form.verifyCode
    }, ReqId.REGISTER, Modules.REGISTERMOD);
  });
};

const goToLogin = () => {
  router.push('/login');
};
</script>

<style lang="scss" scoped>
.register-container {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  background-color: #f5f7fa;

  .register-card {
    width: 500px;
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

  :deep(.el-form-item__content) {
    display: flex;
    justify-content: center;
    gap: 10px;
  }
}
</style> 