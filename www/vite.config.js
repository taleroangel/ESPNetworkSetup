import { defineConfig } from 'vite'
import { viteSingleFile } from 'vite-plugin-singlefile';
import vue from '@vitejs/plugin-vue'

const path = require('path');

export default defineConfig({
	resolve: {
		alias: {
			'@': path.resolve(__dirname, './src')
		},
	},
	build: {
		minify: true,
	},
	css: {
		preprocessorOptions: {
			scss: {
				additionalData: `@import "@/styles/global.scss";`
			}
		}
	},
	plugins: [vue(), viteSingleFile()],
	server: {
		open: '/setup/'
	}
})
