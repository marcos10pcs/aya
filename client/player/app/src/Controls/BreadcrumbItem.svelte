<script>
    export let active = false
</script>

<div class="dark:text-white {active ? '' : 'me-2'}">
    <slot />

    {#if !active}
        <i class="fa-light fa-chevron-right fa-fw text-neutral-500" />
    {/if}
</div>
