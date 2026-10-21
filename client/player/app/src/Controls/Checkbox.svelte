<script>
    import { createEventDispatcher } from "svelte"
    export let checked = false
    const dispatch = createEventDispatcher()
    const fireChanged = () => dispatch("change", checked)
</script>

<label class="flex items-center">
    <input type="checkbox" bind:checked on:change={fireChanged} {...$$props} class="ksk-anim-pop cursor-pointer rounded border-gray-300 text-ksk-500 shadow-sm transition duration-100 focus:ring-ksk-400 dark:border-zinc-700 dark:bg-zinc-900 {$$props.class ?? ''}" />
    {#if $$props.label}
        <span class="ml-2 text-gray-700 dark:text-gray-300 {$$props.labelClass ?? ''}">{$$props.label}</span>
    {:else if $$slots.default}
        <span class="ml-2 flex items-center text-gray-700 dark:text-gray-300 {$$props.labelClass ?? ''}"><slot /></span>
    {/if}
</label>
