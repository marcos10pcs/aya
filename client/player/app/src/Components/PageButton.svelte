<script>
    export let page
    export let selectedPage
    export let onClick
</script>

<button on:click={onClick} data-page={page.id} title={page.title} class={page.id == selectedPage ? "aya-page-btn-selected" : "aya-page-btn"}>
    {#if page.id == "about" || page.id == "settings"}
        <i class="{page.id == selectedPage ? 'fa-solid' : 'fa-regular fa-light'} {page.icon} fa-fw fa-lg text-zinc-800 dark:text-neutral-50" />
    {:else}
        <img src="./img/icons/{page.id}.png" alt={page.title} />
    {/if}
</button>
