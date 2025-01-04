<template>
  <div class="begin-container">
    <div class="content">
      <h1>欢迎使用 HungerYet</h1>
      <p class="subtitle">现代化的餐饮服务系统</p>
      
      <div class="features">
        <el-row :gutter="20">
          <el-col :span="8">
            <el-card>
              <template #header>
                <div class="card-header">
                  <i class="el-icon-food"></i>
                  <span>便捷点餐</span>
                </div>
              </template>
              <div class="card-content">
                快速浏览菜单，轻松完成点餐
              </div>
            </el-card>
          </el-col>
          
          <el-col :span="8">
            <el-card>
              <template #header>
                <div class="card-header">
                  <i class="el-icon-timer"></i>
                  <span>实时追踪</span>
                </div>
              </template>
              <div class="card-content">
                订单状态实时更新，进度一目了然
              </div>
            </el-card>
          </el-col>
          
          <el-col :span="8">
            <el-card>
              <template #header>
                <div class="card-header">
                  <i class="el-icon-wallet"></i>
                  <span>安全支付</span>
                </div>
              </template>
              <div class="card-content">
                多种支付方式，安全便捷
              </div>
            </el-card>
          </el-col>
        </el-row>
      </div>

      <div class="actions">
        <el-button type="primary" size="large" @click="goToLogin">
          立即登录
        </el-button>
        <el-button size="large" @click="goToRegister">
          注册账号
        </el-button>
      </div>
    </div>
  </div>
</template>

<script lang="ts" setup>
import { useRouter } from 'vue-router';

const router = useRouter();

const goToLogin = () => {
  router.push('/login');
};

const goToRegister = () => {
  router.push('/register');
};
</script>

<style lang="scss" scoped>
.begin-container {
  min-height: 100vh;
  display: flex;
  justify-content: center;
  align-items: center;
  background: linear-gradient(135deg, #f5f7fa 0%, #c3cfe2 100%);
  padding: 20px;

  .content {
    text-align: center;
    max-width: 1200px;
    width: 100%;

    h1 {
      font-size: 3em;
      color: #2c3e50;
      margin-bottom: 10px;
    }

    .subtitle {
      font-size: 1.5em;
      color: #34495e;
      margin-bottom: 40px;
    }

    .features {
      margin-bottom: 40px;

      .el-card {
        height: 100%;
        transition: transform 0.3s;

        &:hover {
          transform: translateY(-5px);
        }
      }

      .card-header {
        display: flex;
        align-items: center;
        justify-content: center;
        font-size: 1.2em;
        
        i {
          margin-right: 8px;
        }
      }

      .card-content {
        padding: 20px 0;
        color: #606266;
      }
    }

    .actions {
      .el-button {
        margin: 0 10px;
        padding: 12px 30px;
      }
    }
  }
}
</style> 