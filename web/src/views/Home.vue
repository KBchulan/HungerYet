<template>
  <div class="home-container">
    <div class="service-grid">
      <!-- 状态服务 -->
      <div class="service-card">
        <div class="icon-container">
          <i class="icon status-icon">✓</i>
        </div>
        <h3>状态服务</h3>
        <div class="path">/status/</div>
      </div>

      <!-- 购买服务 -->
      <div class="service-card">
        <div class="icon-container">
          <i class="icon purchase-icon">🛒</i>
        </div>
        <h3>购买服务</h3>
        <div class="path">/purchase/</div>
      </div>

      <!-- 验证服务 -->
      <div class="service-card">
        <div class="icon-container">
          <i class="icon verify-icon">✓</i>
        </div>
        <h3>验证服务</h3>
        <div class="path">/verify/</div>
      </div>

      <!-- Swagger 文档 -->
      <div class="service-card">
        <div class="icon-container">
          <i class="icon docs-icon">📄</i>
        </div>
        <h3>Swagger 文档</h3>
        <div class="path">查看接口文档</div>
      </div>
    </div>
    <footer class="footer">
      © 2024 HungerYet · 让美食触手可及
    </footer>
  </div>
</template>

<script lang="ts">
import { defineComponent } from 'vue'

export default defineComponent({
  name: 'Home'
})
</script>

<style scoped>
.home-container {
  min-height: 100vh;
  padding: 2rem;
  background-color: #fff5ee;
  display: flex;
  flex-direction: column;
}

.service-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 2rem;
  max-width: 1200px;
  margin: 0 auto;
  padding: 2rem;
  flex-grow: 1;
}

.service-card {
  background: white;
  border-radius: 12px;
  padding: 2rem;
  text-align: center;
  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  transition: transform 0.3s ease;
  cursor: pointer;
}

.service-card:hover {
  transform: translateY(-5px);
}

.icon-container {
  margin-bottom: 1rem;
}

.icon {
  font-size: 2rem;
  width: 64px;
  height: 64px;
  display: flex;
  align-items: center;
  justify-content: center;
  margin: 0 auto;
  border-radius: 50%;
}

.status-icon {
  background-color: #e3f2fd;
  color: #1976d2;
}

.purchase-icon {
  background-color: #fff3e0;
  color: #f57c00;
}

.verify-icon {
  background-color: #e8f5e9;
  color: #388e3c;
}

.docs-icon {
  background-color: #fce4ec;
  color: #c2185b;
}

h3 {
  margin: 1rem 0;
  color: #333;
  font-size: 1.5rem;
}

.path {
  color: #666;
  font-family: monospace;
  background-color: #f5f5f5;
  padding: 0.5rem;
  border-radius: 4px;
  margin-top: 1rem;
}

.footer {
  text-align: center;
  padding: 1rem;
  color: #666;
  margin-top: auto;
}
</style> 