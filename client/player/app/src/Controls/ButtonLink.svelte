<script>
    export let href
    export let text
</script>

<button {href} class="ksk-anim-pop rounded-lg px-2 py-1 text-neutral-500 underline transition duration-200 hover:bg-black/10 hover:text-neutral-600 dark:text-zinc-500 dark:hover:bg-white/10 dark:hover:text-zinc-400 {$$props.class ?? ''}">
    {#if !text}
        <slot />
    {:else}
        {text}
    {/if}
</button>
