import defaultTheme from "tailwindcss/defaultTheme"
import forms from "@tailwindcss/forms"
import typography from "@tailwindcss/typography"

/** @type {import('tailwindcss').Config} */
export default {
    content: ["./client/player/app/src/**/*.{html,js,svelte,ts}"],
    darkMode: ["selector", '[data-mode="dark"]'],
    theme: {
        extend: {
            fontFamily: {
                sans: ["'Source Sans 3'", "Figtree", "'Twemoji Mozilla'", "icomoon", ...defaultTheme.fontFamily.sans],
                mono: ["'Source Code Pro'", "'Code New Roman'", "'Twemoji Mozilla'", "icomoon", ...defaultTheme.fontFamily.mono],
                chomsky: ["Chomsky", "'Twemoji Mozilla'", "icomoon", ...defaultTheme.fontFamily.sans]
            },
            colors: {
                ksk: {
                    100: "#e7d9fd",
                    200: "#ceb3fb",
                    300: "#b68cf9",
                    400: "#9d66f7",
                    500: "#8540f5",
                    600: "#6a33c4",
                    700: "#502693",
                    800: "#351a62",
                    900: "#1b0d31"
                }
            }
        }
    },

    variants: {
        extend: {
            backgroundColor: ["active", "focus"]
        }
    },

    plugins: [forms, typography]
}
