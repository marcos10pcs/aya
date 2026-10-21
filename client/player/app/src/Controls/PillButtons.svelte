<script>
    import { createEventDispatcher } from "svelte"

    export let items = []
    export let activeItem = items[0]?.id

    const dispatch = createEventDispatcher()

    function handleItemChange(id) {
        activeItem = id
        dispatch("itemChange", {id})
    }
</script>

<div class="flex">
    {#each items as item}
        <button class="ksk-anim-pop me-1 rounded-full px-3 py-1 text-sm transition duration-200 {activeItem === item.id ? 'border border-ksk-500 bg-white font-medium text-ksk-500 hover:bg-black/5 dark:border-ksk-400 dark:bg-neutral-800 dark:text-ksk-400' : 'text-neutral-600 hover:bg-black/10 dark:text-neutral-400 dark:hover:bg-white/10'}" class:active={activeItem === item.id} on:click={() => handleItemChange(item.id)}>
            <i class={`${activeItem === item.id ? "fa-solid" : "fa-regular"} ${item.icon} fa-fw`} />
            {item.label}
        </button>
    {/each}
</div>
