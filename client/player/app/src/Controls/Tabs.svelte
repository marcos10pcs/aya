<script>
    import { onMount, createEventDispatcher } from "svelte"

    export let tabs = []
    export let activeTab
    export let hideTabs = false
    export let roundedCorners = true

    let tabRefs = {}
    let lineStyle = ""
    const dispatch = createEventDispatcher()

    function setActiveTab(id) {
        activeTab = id
        dispatch("tabChange", id)
        updateLine()
    }

    function updateLine() {
        const activeTabRef = tabRefs[activeTab]
        if (activeTabRef) {
            lineStyle = `left: ${activeTabRef.offsetLeft}px; width: ${activeTabRef.offsetWidth}px;`
        }
    }

    function handleResize() {
        updateLine()
    }

    $: tabClasses = tabs.map((tab, index) => {
        let classes = ["ksk-anim-pop", "px-6", "py-2", "text-lg", "transition", "duration-200", "hover:bg-black/5", "dark:hover:bg-white/5", $$props.class ?? ""]

        if (activeTab === tab.id) {
            classes.push("font-medium", "text-ksk-500")
        } else {
            classes.push("text-neutral-500", "dark:text-neutral-400", "hover:text-neutral-700", "dark:hover:text-neutral-300")
        }

        if (roundedCorners) {
            classes.push("rounded-t-lg")
        } else {
            if (index === 0) classes.push("rounded-tl-lg")
            if (index === tabs.length - 1) classes.push("rounded-tr-lg")
            if (index !== 0 && index !== tabs.length - 1) classes.push("border-t-0")
        }

        return classes.join(" ")
    })

    onMount(() => {
        updateLine()
    })

    $: activeTab, hideTabs, updateLine()
</script>

<svelte:window on:resize={handleResize} />

<div class="relative mb-2 flex {$$props.class ?? ''}">
    {#each tabs as tab, index}
        <button bind:this={tabRefs[tab.id]} on:click={() => setActiveTab(tab.id)} class={tabClasses[index]}>
            {tab.label}
        </button>
    {/each}
    {#if !hideTabs}
        <span class="absolute bottom-0 h-0.5 rounded bg-ksk-500 transition-all duration-200 ease-in-out" style={lineStyle}></span>
    {/if}
    {#if $$slots.header}
        <slot name="header" />
    {/if}
</div>

<slot {activeTab}></slot>
