<script>
    import { createEventDispatcher } from "svelte"

    export let items = []
    export let activeItem = items[0]?.id

    const dispatch = createEventDispatcher()

    function handleItemChange(id) {
        activeItem = id
        dispatch("itemChange", id)
    }
</script>

{#each items as item}
    <button class="ksk-anim-pop flex flex-nowrap items-center rounded-lg px-2 py-1 text-start transition duration-200 {activeItem === item.id ? 'bg-black/10 dark:bg-white/10 font-semibold text-ksk-700 dark:text-ksk-500' : 'text-ksk-600/85 hover:bg-black/5 dark:hover:bg-white/5 hover:text-ksk-600 dark:text-ksk-600'}" class:active={activeItem === item.id} on:click={() => handleItemChange(item.id)}>
        <i class={`${activeItem === item.id ? "fa-solid" : "fa-regular"} ${item.icon} fa-fw me-1`} />
        {item.label}
    </button>
{/each}
