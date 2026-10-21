import { defineConfig } from "vite"
import { svelte } from "@sveltejs/vite-plugin-svelte"
import path from "path"

export default defineConfig({
    root: path.resolve(__dirname, "client/player/app"),
    base: "./",
    resolve: {
        alias: {
            "@": path.resolve(__dirname, "client/player/app/src")
        }
    },
    build: {
        outDir: path.resolve(__dirname, "client/resources/content/app"),
        emptyOutDir: true,
        rollupOptions: {
            output: {
                manualChunks: undefined,
                inlineDynamicImports: true
            }
        }
    },
    plugins: [svelte()]
})
