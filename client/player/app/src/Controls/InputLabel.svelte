<script>
    export let value = null
</script>

<label class="select-none dark:text-neutral-100" for={$$props.for} {...$$props}>
    {#if value}
        <span>{value}</span>
    {:else}
        <span><slot /></span>
    {/if}
</label>
