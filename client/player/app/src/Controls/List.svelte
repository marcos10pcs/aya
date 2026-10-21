<script>
    import { createEventDispatcher, onMount } from "svelte"

    export let items = []
    export let activeItem = items[0]?.id

    const dispatch = createEventDispatcher()

    function handleItemChange(id) {
        activeItem = id
        dispatch("itemChange", id)
    }
</script>

<div class="flex flex-col">
    {#each items as item}
        {#if activeItem === item.id}
            <div class="flex items-center">
                <button class="ksk-games-genre-list w-full !ms-0"><i class={`fa-solid fa-caret-right me-1 text-red-500`} /> {item.label}</button>
            </div>
        {:else}
            <button class={`ksk-games-genre-list ${!item.icon ? 'ml-[10px]' : ''}`} on:click={() => handleItemChange(item.id)}>
                {#if item.icon}
                    <i class={`fa-light fa-fw me-1 ${item.icon}`} />
                {/if}
                {item.label}
            </button>
        {/if}
    {/each}
</div>