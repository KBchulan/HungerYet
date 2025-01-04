/// <reference types="vite/client" />

declare module '*.vue' {
  import type { DefineComponent } from 'vue'
  const component: DefineComponent<{}, {}, any>
  export default component
}

// 扩展 Window 接口
declare interface Window {
  // 在这里添加全局变量
}

// 声明静态资源模块
declare module '*.svg' {
  const path: string
  export default path
}

declare module '*.png' {
  const path: string
  export default path
}

declare module '*.jpg' {
  const path: string
  export default path
}

declare module '*.jpeg' {
  const path: string
  export default path
}

declare module '*.gif' {
  const path: string
  export default path
}

declare module '*.webp' {
  const path: string
  export default path
} 