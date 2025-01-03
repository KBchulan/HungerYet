<template>
  <div class="profile-container">
    <div class="profile-content">
      <div class="profile-header">
        <div class="avatar-section">
          <img :src="userInfo.avatar || '/images/default-avatar.png'" alt="用户头像" class="avatar">
          <button class="change-avatar-btn">更换头像</button>
        </div>
        <div class="user-info">
          <h2>{{ userInfo.username }}</h2>
          <p class="email">{{ userInfo.email }}</p>
        </div>
      </div>

      <div class="profile-sections">
        <div class="section">
          <h3>基本信息</h3>
          <form @submit.prevent="updateProfile" class="profile-form">
            <div class="form-group">
              <label for="username">用户名</label>
              <input
                type="text"
                id="username"
                v-model="form.username"
                placeholder="请输入用户名"
              >
            </div>
            <div class="form-group">
              <label for="phone">手机号</label>
              <input
                type="tel"
                id="phone"
                v-model="form.phone"
                placeholder="请输入手机号"
              >
            </div>
            <div class="form-group">
              <label for="address">默认地址</label>
              <textarea
                id="address"
                v-model="form.address"
                placeholder="请输入默认地址"
                rows="3"
              ></textarea>
            </div>
            <button type="submit" class="save-btn">保存修改</button>
          </form>
        </div>

        <div class="section">
          <h3>账户安全</h3>
          <div class="security-items">
            <div class="security-item">
              <div class="security-info">
                <h4>登录密码</h4>
                <p>建议定期更换密码，保证账户安全</p>
              </div>
              <button @click="showChangePassword = true" class="change-btn">
                修改
              </button>
            </div>
            <div class="security-item">
              <div class="security-info">
                <h4>绑定手机</h4>
                <p>已绑定：{{ userInfo.phone || '未绑定' }}</p>
              </div>
              <button @click="showBindPhone = true" class="change-btn">
                {{ userInfo.phone ? '更换' : '绑定' }}
              </button>
            </div>
          </div>
        </div>

        <div class="section">
          <h3>消息通知</h3>
          <div class="notification-settings">
            <div class="notification-item">
              <span>订单状态通知</span>
              <label class="switch">
                <input type="checkbox" v-model="notifications.order">
                <span class="slider"></span>
              </label>
            </div>
            <div class="notification-item">
              <span>优惠活动通知</span>
              <label class="switch">
                <input type="checkbox" v-model="notifications.promotion">
                <span class="slider"></span>
              </label>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script lang="ts">
import { defineComponent, ref, reactive } from 'vue'

interface UserInfo {
  username: string
  email: string
  phone?: string
  avatar?: string
  address?: string
}

export default defineComponent({
  name: 'Profile',
  setup() {
    const userInfo = reactive<UserInfo>({
      username: '测试用户',
      email: 'test@example.com',
      phone: '13800138000',
      address: '北京市朝阳区xxx街道xxx号'
    })

    const form = reactive({
      username: userInfo.username,
      phone: userInfo.phone || '',
      address: userInfo.address || ''
    })

    const notifications = reactive({
      order: true,
      promotion: false
    })

    const showChangePassword = ref(false)
    const showBindPhone = ref(false)

    const updateProfile = async () => {
      try {
        // TODO: 实现更新个人信息的逻辑
        console.log('更新个人信息：', form)
        Object.assign(userInfo, form)
      } catch (error) {
        console.error('更新失败：', error)
      }
    }

    return {
      userInfo,
      form,
      notifications,
      showChangePassword,
      showBindPhone,
      updateProfile
    }
  }
})
</script>

<style scoped>
.profile-container {
  padding: 2rem;
  max-width: 1200px;
  margin: 0 auto;
}

.profile-content {
  background: white;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
  padding: 2rem;
}

.profile-header {
  display: flex;
  gap: 2rem;
  margin-bottom: 2rem;
  padding-bottom: 2rem;
  border-bottom: 1px solid #eee;
}

.avatar-section {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 1rem;
}

.avatar {
  width: 120px;
  height: 120px;
  border-radius: 50%;
  object-fit: cover;
}

.change-avatar-btn {
  padding: 0.5rem 1rem;
  background: none;
  border: 1px solid #4CAF50;
  color: #4CAF50;
  border-radius: 4px;
  cursor: pointer;
}

.user-info {
  display: flex;
  flex-direction: column;
  justify-content: center;
}

.user-info h2 {
  margin: 0 0 0.5rem 0;
}

.email {
  color: #666;
}

.profile-sections {
  display: flex;
  flex-direction: column;
  gap: 2rem;
}

.section {
  padding: 1.5rem;
  border: 1px solid #eee;
  border-radius: 8px;
}

.section h3 {
  margin: 0 0 1.5rem 0;
}

.profile-form {
  display: flex;
  flex-direction: column;
  gap: 1.5rem;
}

.form-group {
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
}

.form-group label {
  color: #666;
}

.form-group input,
.form-group textarea {
  padding: 0.75rem;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 1rem;
}

.save-btn {
  padding: 0.75rem;
  background-color: #4CAF50;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 1rem;
}

.security-items {
  display: flex;
  flex-direction: column;
  gap: 1rem;
}

.security-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 1rem;
  border: 1px solid #eee;
  border-radius: 4px;
}

.security-info h4 {
  margin: 0 0 0.5rem 0;
}

.security-info p {
  color: #666;
  margin: 0;
}

.change-btn {
  padding: 0.5rem 1rem;
  background: none;
  border: 1px solid #4CAF50;
  color: #4CAF50;
  border-radius: 4px;
  cursor: pointer;
}

.notification-settings {
  display: flex;
  flex-direction: column;
  gap: 1rem;
}

.notification-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 1rem;
  border: 1px solid #eee;
  border-radius: 4px;
}

.switch {
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
}

.switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #ccc;
  transition: .4s;
  border-radius: 34px;
}

.slider:before {
  position: absolute;
  content: "";
  height: 26px;
  width: 26px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  transition: .4s;
  border-radius: 50%;
}

input:checked + .slider {
  background-color: #4CAF50;
}

input:checked + .slider:before {
  transform: translateX(26px);
}
</style> 