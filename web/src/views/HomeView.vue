<template>
  <div class="home-container">
    <el-container>
      <el-header>
        <div class="header-content">
          <div class="logo">
            <h1>HungerYet</h1>
          </div>
          <div class="user-info">
            <el-dropdown @command="handleCommand">
              <span class="el-dropdown-link">
                {{ userStore.username }}
                <el-icon class="el-icon--right">
                  <arrow-down />
                </el-icon>
              </span>
              <template #dropdown>
                <el-dropdown-menu>
                  <el-dropdown-item command="profile">个人信息</el-dropdown-item>
                  <el-dropdown-item command="orders">我的订单</el-dropdown-item>
                  <el-dropdown-item command="favorites">我的收藏</el-dropdown-item>
                  <el-dropdown-item divided command="logout">退出登录</el-dropdown-item>
                </el-dropdown-menu>
              </template>
            </el-dropdown>
          </div>
        </div>
      </el-header>

      <el-main>
        <div class="main-content">
          <el-row :gutter="20">
            <el-col :span="8">
              <el-card class="feature-card" @click="goToMenu">
                <template #header>
                  <div class="card-header">
                    <el-icon><menu /></el-icon>
                    <span>浏览菜单</span>
                  </div>
                </template>
                <div class="card-content">
                  查看最新菜品，发现美食推荐
                </div>
              </el-card>
            </el-col>

            <el-col :span="8">
              <el-card class="feature-card" @click="goToOrder">
                <template #header>
                  <div class="card-header">
                    <el-icon><shopping-cart /></el-icon>
                    <span>我的订单</span>
                  </div>
                </template>
                <div class="card-content">
                  查看订单状态，追踪配送进度
                </div>
              </el-card>
            </el-col>

            <el-col :span="8">
              <el-card class="feature-card" @click="goToMerchant">
                <template #header>
                  <div class="card-header">
                    <el-icon><shop /></el-icon>
                    <span>商家中心</span>
                  </div>
                </template>
                <div class="card-content">
                  浏览商家信息，查看评价反馈
                </div>
              </el-card>
            </el-col>
          </el-row>

          <el-row :gutter="20" class="mt-20" v-if="userStore.isAdmin">
            <el-col :span="8">
              <el-card class="feature-card" @click="goToAdmin">
                <template #header>
                  <div class="card-header">
                    <el-icon><setting /></el-icon>
                    <span>管理后台</span>
                  </div>
                </template>
                <div class="card-content">
                  系统管理，数据统计
                </div>
              </el-card>
            </el-col>
          </el-row>
        </div>
      </el-main>

      <el-footer>
        <p>© 2024 HungerYet. All rights reserved.</p>
      </el-footer>
    </el-container>
  </div>
</template>

<script lang="ts" setup>
import { useRouter } from 'vue-router';
import { useUserStore } from '../stores/userStore';
import { ArrowDown, Menu, ShoppingCart, Shop, Setting } from '@element-plus/icons-vue';
import { ElMessageBox } from 'element-plus';

const router = useRouter();
const userStore = useUserStore();

const handleCommand = (command: string) => {
  switch (command) {
    case 'profile':
      // TODO: 实现个人信息页面
      break;
    case 'orders':
      router.push('/order');
      break;
    case 'favorites':
      // TODO: 实现收藏夹页面
      break;
    case 'logout':
      ElMessageBox.confirm(
        '确定要退出登录吗？',
        '提示',
        {
          confirmButtonText: '确定',
          cancelButtonText: '取消',
          type: 'warning',
        }
      ).then(() => {
        userStore.logout();
        router.push('/login');
      }).catch(() => {});
      break;
  }
};

const goToMenu = () => {
  router.push('/menu');
};

const goToOrder = () => {
  router.push('/order');
};

const goToMerchant = () => {
  router.push('/merchant');
};

const goToAdmin = () => {
  router.push('/admin');
};
</script>

<style lang="scss" scoped>
.home-container {
  min-height: 100vh;
  background-color: #f5f7fa;

  .el-container {
    min-height: 100vh;
  }

  .el-header {
    background-color: #fff;
    box-shadow: 0 2px 12px 0 rgba(0, 0, 0, 0.1);
    position: fixed;
    width: 100%;
    z-index: 100;
    padding: 0;

    .header-content {
      max-width: 1200px;
      margin: 0 auto;
      height: 60px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 0 20px;

      .logo {
        h1 {
          margin: 0;
          color: #409EFF;
        }
      }

      .user-info {
        .el-dropdown-link {
          cursor: pointer;
          display: flex;
          align-items: center;
          color: #606266;

          &:hover {
            color: #409EFF;
          }
        }
      }
    }
  }

  .el-main {
    padding-top: 80px;
    
    .main-content {
      max-width: 1200px;
      margin: 0 auto;
      padding: 20px;

      .feature-card {
        cursor: pointer;
        transition: transform 0.3s;

        &:hover {
          transform: translateY(-5px);
        }

        .card-header {
          display: flex;
          align-items: center;
          font-size: 1.2em;

          .el-icon {
            margin-right: 8px;
            font-size: 1.2em;
          }
        }

        .card-content {
          color: #606266;
          padding: 20px 0;
        }
      }
    }
  }

  .el-footer {
    text-align: center;
    color: #909399;
    padding: 20px;
    background-color: #fff;
  }
}

.mt-20 {
  margin-top: 20px;
}
</style> 