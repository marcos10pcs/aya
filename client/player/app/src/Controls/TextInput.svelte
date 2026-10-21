<script>
    import { onMount } from "svelte"

    export let value = ""
    export let error = null
    export let icon = null

    let inputElement
    let isFocused = false

    // force focus just in case
    onMount(() => {
        if (inputElement.hasAttribute("autofocus")) {
            inputElement.focus()
        }
    })

    $: classes = [error ? "pe-7 dark:border-red-400 border-red-600 focus:border-red-500 focus:ring-red-600 hover:border-red-400 dark:hover:border-red-600" : "border-gray-300 hover:border-ksk-300 focus:border-ksk-500 dark:border-neutral-700 dark:focus:border-ksk-600", `focus:ring-ksk-500 block w-full rounded border-gray-300 transition duration-100 dark:bg-neutral-900 dark:text-neutral-300 h-[2rem] dark:placeholder-neutral-500 ${$$props.class ?? ""} ${icon ? "pl-10" : ""}`].filter(Boolean).join(" ")
    $: iconColor = isFocused ? "text-red-500 dark:text-red-600" : "text-red-600 dark:text-red-400"

    // hack
    let restProps = $$restProps
    if (restProps.class) {
        delete restProps.class
    }
</script>

<div class="relative {$$props.class && $$props.class.includes('w-full') ? 'w-full' : ''}">
    {#if icon}
        <i class={`fa-regular fa-fw ${icon} pointer-events-none absolute left-3 top-1/2 -translate-y-1/2 text-neutral-400`} />
    {/if}
    <input class={classes} bind:value bind:this={inputElement} on:focus={() => (isFocused = true)} on:blur={() => (isFocused = false)} {...restProps} />
    {#if error}
        <i class={`fa-solid fa-times pointer-events-none absolute right-3 top-1/2 -translate-y-1/2 transform transition duration-100 ${iconColor}`} />
    {/if}
</div>
