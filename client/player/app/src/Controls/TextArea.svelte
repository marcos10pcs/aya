<script>
    import { onMount } from "svelte"

    export let value = ""
    export let error = null

    let textAreaElement
    let isFocused = false

    onMount(() => {
        if (textAreaElement.hasAttribute("autofocus")) {
            textAreaElement.focus()
        }
    })

    $: classes = [error ? "pe-7 dark:border-red-400 border-red-600 focus:border-red-500 focus:ring-red-600 hover:border-red-400 dark:hover:border-red-600" : "border-gray-300 dark:border-gray-700 hover:border-ksk-300 focus:border-ksk-500 dark:border-zinc-700 dark:focus:border-ksk-600", $$props.class ?? "", "focus:ring-ksk-500 block w-full rounded-lg border-gray-300 transition duration-100 dark:bg-zinc-900 dark:text-gray-300"].filter(Boolean).join(" ")
    $: iconColor = isFocused ? "text-red-500 dark:text-red-600" : "text-red-600 dark:text-red-400"

    // hack
    let restProps = $$restProps
    if (restProps.class) {
        delete restProps.class
    }
</script>

<div class="relative {$$props.class && $$props.class.includes('w-full') ? 'w-full' : ''}">
    <textarea class={classes} bind:value bind:this={textAreaElement} on:focus={() => (isFocused = true)} on:blur={() => (isFocused = false)} {...restProps} />
    {#if error}
        <i class={`fas fa-times pointer-events-none absolute right-3 top-1/2 -translate-y-1/2 transform transition duration-100 ${iconColor}`} />
    {/if}
</div>
