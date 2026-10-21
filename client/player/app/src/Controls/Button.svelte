<script>
    import { onMount, createEventDispatcher } from "svelte"
    import { writable } from "svelte/store"

    export let icon = null
    export let text = null
    export let disabled = false

    const iconStore = writable(icon)
    const dispatch = createEventDispatcher()

    onMount(() => iconStore.set(`fa-regular ${icon}`))
    const handleMouseEnter = () => {
        if (!disabled) iconStore.set(`fa-solid ${icon}`)
    }
    const handleMouseLeave = () => {
        if (!disabled) iconStore.set(`fa-regular ${icon}`)
    }
    const handleMouseClick = () => {
        if (!disabled) dispatch("click")
    }

    // hack
    let restProps = $$restProps
    if (restProps.class) {
        delete restProps.class
    }
</script>

<button class="ksk-anim-pop inline-flex h-10 min-h-10 items-center justify-center gap-2 rounded-lg bg-ksk-500 px-3 text-lg text-white transition duration-200 hover:bg-ksk-600 disabled:opacity-35 {$$props.class ?? ''}" on:mouseenter={handleMouseEnter} on:mouseleave={handleMouseLeave} on:click={handleMouseClick} disabled={disabled || null} {...restProps}>
    {#if icon}
        <i class={$iconStore} />
    {/if}

    {#if text}
        <span>{text}</span>
    {:else}
        <slot />
    {/if}
</button>
