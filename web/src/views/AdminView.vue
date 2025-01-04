<template>
  <div class="admin-container">
    <el-container>
      <el-aside width="200px">
        <div class="logo">
          <h2>管理后台</h2>
        </div>
        <el-menu
          :default-active="activeMenu"
          class="admin-menu"
          @select="handleMenuSelect"
        >
          <el-menu-item index="dashboard">
            <el-icon><menu /></el-icon>
            <span>仪表盘</span>
          </el-menu-item>
          <el-menu-item index="orders">
            <el-icon><shopping-cart /></el-icon>
            <span>订单管理</span>
          </el-menu-item>
          <el-menu-item index="dishes">
            <el-icon><food /></el-icon>
            <span>菜品管理</span>
          </el-menu-item>
          <el-menu-item index="merchants">
            <el-icon><shop /></el-icon>
            <span>商家管理</span>
          </el-menu-item>
          <el-menu-item index="users">
            <el-icon><user /></el-icon>
            <span>用户管理</span>
          </el-menu-item>
          <el-menu-item index="settings">
            <el-icon><setting /></el-icon>
            <span>系统设置</span>
          </el-menu-item>
        </el-menu>
      </el-aside>

      <el-container>
        <el-header>
          <div class="header-content">
            <div class="breadcrumb">
              <el-breadcrumb>
                <el-breadcrumb-item>首页</el-breadcrumb-item>
                <el-breadcrumb-item>{{ getMenuTitle(activeMenu) }}</el-breadcrumb-item>
              </el-breadcrumb>
            </div>
            <div class="user-actions">
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
                    <el-dropdown-item command="home">返回首页</el-dropdown-item>
                    <el-dropdown-item divided command="logout">退出登录</el-dropdown-item>
                  </el-dropdown-menu>
                </template>
              </el-dropdown>
            </div>
          </div>
        </el-header>

        <el-main>
          <!-- 仪表盘 -->
          <div v-if="activeMenu === 'dashboard'" class="dashboard">
            <el-row :gutter="20">
              <el-col :span="6">
                <el-card class="data-card">
                  <template #header>
                    <div class="card-header">
                      <span>今日订单</span>
                      <el-tag size="small">实时</el-tag>
                    </div>
                  </template>
                  <div class="card-content">
                    <div class="number">{{ dashboardData.todayOrders }}</div>
                    <div class="trend">
                      较昨日
                      <span :class="dashboardData.orderTrend >= 0 ? 'up' : 'down'">
                        {{ Math.abs(dashboardData.orderTrend) }}%
                      </span>
                    </div>
                  </div>
                </el-card>
              </el-col>

              <el-col :span="6">
                <el-card class="data-card">
                  <template #header>
                    <div class="card-header">
                      <span>今日营收</span>
                      <el-tag size="small" type="success">实时</el-tag>
                    </div>
                  </template>
                  <div class="card-content">
                    <div class="number">¥{{ dashboardData.todayRevenue }}</div>
                    <div class="trend">
                      较昨日
                      <span :class="dashboardData.revenueTrend >= 0 ? 'up' : 'down'">
                        {{ Math.abs(dashboardData.revenueTrend) }}%
                      </span>
                    </div>
                  </div>
                </el-card>
              </el-col>

              <el-col :span="6">
                <el-card class="data-card">
                  <template #header>
                    <div class="card-header">
                      <span>新增用户</span>
                      <el-tag size="small" type="warning">今日</el-tag>
                    </div>
                  </template>
                  <div class="card-content">
                    <div class="number">{{ dashboardData.newUsers }}</div>
                    <div class="trend">
                      较昨日
                      <span :class="dashboardData.userTrend >= 0 ? 'up' : 'down'">
                        {{ Math.abs(dashboardData.userTrend) }}%
                      </span>
                    </div>
                  </div>
                </el-card>
              </el-col>

              <el-col :span="6">
                <el-card class="data-card">
                  <template #header>
                    <div class="card-header">
                      <span>活跃商家</span>
                      <el-tag size="small" type="info">今日</el-tag>
                    </div>
                  </template>
                  <div class="card-content">
                    <div class="number">{{ dashboardData.activeMerchants }}</div>
                    <div class="trend">
                      较昨日
                      <span :class="dashboardData.merchantTrend >= 0 ? 'up' : 'down'">
                        {{ Math.abs(dashboardData.merchantTrend) }}%
                      </span>
                    </div>
                  </div>
                </el-card>
              </el-col>
            </el-row>

            <!-- 图表区域 -->
            <el-row :gutter="20" class="charts">
              <el-col :span="12">
                <el-card>
                  <template #header>
                    <div class="card-header">
                      <span>订单趋势</span>
                    </div>
                  </template>
                  <!-- TODO: 添加订单趋势图表 -->
                </el-card>
              </el-col>

              <el-col :span="12">
                <el-card>
                  <template #header>
                    <div class="card-header">
                      <span>营收趋势</span>
                    </div>
                  </template>
                  <!-- TODO: 添加营收趋势图表 -->
                </el-card>
              </el-col>
            </el-row>
          </div>

          <!-- 订单管理 -->
          <div v-else-if="activeMenu === 'orders'" class="orders-management">
            <!-- TODO: 实现订单管理界面 -->
          </div>

          <!-- 菜品管理 -->
          <div v-else-if="activeMenu === 'dishes'" class="dishes-management">
            <!-- TODO: 实现菜品管理界面 -->
          </div>

          <!-- 商家管理 -->
          <div v-else-if="activeMenu === 'merchants'" class="merchants-management">
            <!-- TODO: 实现商家管理界面 -->
          </div>

          <!-- 用户管理 -->
          <div v-else-if="activeMenu === 'users'" class="users-management">
            <!-- TODO: 实现用户管理界面 -->
          </div>

          <!-- 系统设置 -->
          <div v-else-if="activeMenu === 'settings'" class="system-settings">
            <!-- TODO: 实现系统设置界面 -->
          </div>
        </el-main>
      </el-container>
    </el-container>
  </div>
</template>

<script lang="ts" setup>
import { ref, onMounted } from 'vue';
import { useRouter } from 'vue-router';
import { useUserStore } from '../stores/userStore';
import { ElMessage, ElMessageBox } from 'element-plus';
import { Menu, ShoppingCart, Food, Shop, User, Setting, ArrowDown } from '@element-plus/icons-vue';
import HttpManager, { ReqId, Modules, ErrorCodes } from '../services/HttpManager';

const router = useRouter();
const userStore = useUserStore();
const httpManager = HttpManager.getInstance();

// 状态
const activeMenu = ref('dashboard');
const dashboardData = ref({
  todayOrders: 0,
  orderTrend: 0,
  todayRevenue: 0,
  revenueTrend: 0,
  newUsers: 0,
  userTrend: 0,
  activeMerchants: 0,
  merchantTrend: 0
});

// 方法
const getMenuTitle = (menu: string) => {
  const titles = {
    dashboard: '仪表盘',
    orders: '订单管理',
    dishes: '菜品管理',
    merchants: '商家管理',
    users: '用户管理',
    settings: '系统设置'
  };
  return titles[menu as keyof typeof titles] || menu;
};

const handleMenuSelect = (index: string) => {
  activeMenu.value = index;
};

const handleCommand = (command: string) => {
  switch (command) {
    case 'profile':
      // TODO: 实现个人信息页面
      break;
    case 'home':
      router.push('/home');
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

const loadDashboardData = async () => {
  try {
    const response = await new Promise((resolve) => {
      const handleResponse = (id: ReqId, res: string, err: ErrorCodes, mod: Modules) => {
        if (err === ErrorCodes.SUCCESS) {
          const response = JSON.parse(res);
          resolve(response);
        } else {
          ElMessage.error('网络错误，请稍后重试');
          resolve(null);
        }
        httpManager.off('httpFinished', handleResponse);
      };

      httpManager.on('httpFinished', handleResponse);
      httpManager.postHttpReq('/admin/dashboard', {}, ReqId.ADMIN, Modules.ADMINMOD);
    });

    if (response && response.code === 200) {
      dashboardData.value = response.data;
    }
  } catch (error) {
    ElMessage.error('获取仪表盘数据失败');
  }
};

// 生命周期
onMounted(() => {
  loadDashboardData();
});
</script>

<style lang="scss" scoped>
.admin-container {
  min-height: 100vh;

  .el-container {
    min-height: 100vh;
  }

  .el-aside {
    background-color: #304156;
    color: #fff;

    .logo {
      height: 60px;
      display: flex;
      align-items: center;
      justify-content: center;
      border-bottom: 1px solid #1f2d3d;

      h2 {
        margin: 0;
        color: #fff;
      }
    }

    .admin-menu {
      border-right: none;
      background-color: transparent;

      :deep(.el-menu-item) {
        color: #bfcbd9;

        &:hover {
          color: #fff;
          background-color: #263445;
        }

        &.is-active {
          color: #409EFF;
          background-color: #263445;
        }

        .el-icon {
          margin-right: 10px;
        }
      }
    }
  }

  .el-header {
    background-color: #fff;
    box-shadow: 0 1px 4px rgba(0, 21, 41, 0.08);
    padding: 0 20px;

    .header-content {
      height: 60px;
      display: flex;
      justify-content: space-between;
      align-items: center;

      .user-actions {
        .el-dropdown-link {
          cursor: pointer;
          display: flex;
          align-items: center;
          color: #606266;

          &:hover {
            color: #409EFF;
          }

          .el-icon {
            margin-left: 5px;
          }
        }
      }
    }
  }

  .el-main {
    background-color: #f0f2f5;
    padding: 20px;

    .dashboard {
      .data-card {
        .card-header {
          display: flex;
          justify-content: space-between;
          align-items: center;
        }

        .card-content {
          text-align: center;
          padding: 20px 0;

          .number {
            font-size: 24px;
            font-weight: bold;
            color: #303133;
            margin-bottom: 10px;
          }

          .trend {
            color: #909399;
            font-size: 14px;

            .up {
              color: #67c23a;
            }

            .down {
              color: #f56c6c;
            }
          }
        }
      }

      .charts {
        margin-top: 20px;

        .el-card {
          height: 400px;
        }
      }
    }
  }
}
</style> 