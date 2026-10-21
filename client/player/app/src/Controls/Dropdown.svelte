<script>
    import { onMount, setContext } from "svelte"
    import { fade } from "svelte/transition"

    import DropdownButton from "@/Controls/DropdownButton.svelte"

    let shown = false
    let dropdownRef

    setContext("dropdown", {
        close: () => (shown = false),
        isShown: () => shown
    })

    onMount(() => {
        const handleClickOutside = (event) => {
            if (dropdownRef && !dropdownRef.contains(event.target) && shown) {
                shown = false
            }
        }

        window.addEventListener("click", handleClickOutside)

        return () => {
            window.removeEventListener("click", handleClickOutside)
        }
    })

    const toggleDropdown = () => (shown = !shown)
</script>

<div bind:this={dropdownRef} class="relative">
    {#if $$slots.button}
        <slot name="button" {shown} {toggleDropdown} />
    {:else}
        <DropdownButton {shown} onClick={toggleDropdown} />
    {/if}

    {#if shown}
        <div class="absolute z-10 mt-2 w-48 rounded-md bg-white dark:bg-zinc-800 shadow-lg ring-1 ring-black ring-opacity-5 {$$props.class ?? ''}" transition:fade={{ duration: 50 }}>
            <div class="py-1" role="menu" aria-orientation="vertical" aria-labelledby="options-menu">
                <slot />
            </div>
        </div>
    {/if}
</div>
