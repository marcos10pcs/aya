<script>
    export let value = ""
</script>

<select bind:value class="mt-1 w-full rounded-lg dark:bg-zinc-900 border-gray-300 dark:border-neutral-700 text-neutral-700 dark:text-neutral-300 transition duration-100 focus:border-ksk-500">
    <slot />
</select>
