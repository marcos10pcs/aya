<script>
    import { getContext } from "svelte"
    export let text = null
    export let icon = null
    export let iconPosition = "left"
    export let style = null // "danger"
    export let selected = false
    export let onClick = () => {}

    const { close } = getContext("dropdown")

    function handleClick() {
        onClick()
        close()
    }
</script>

<button on:click={handleClick} class="ksk-anim-pop flex w-full items-center px-3 py-1 text-sm text-neutral-700 transition duration-100 dark:text-neutral-300 {style === 'danger' ? 'hover:bg-red-100 hover:text-red-900 dark:hover:bg-red-950 dark:hover:text-red-100' : 'hover:bg-gray-100 dark:hover:bg-zinc-700'} {icon && iconPosition === 'right' ? 'justify-between' : ''} {selected ? 'bg-gray-200 hover:bg-gray-300' : ''} {$$props.class ?? ''}" role="menuitem">
    {#if icon && iconPosition === "left"}
        <i class="{selected ? 'fa-solid' : 'fa-light'} {icon} fa-fw me-1" />
    {/if}
    <span class={selected ? "font-semibold" : "font-normal"}>{text}</span>
    {#if icon && iconPosition === "right"}
        <i class="{selected ? 'fa-solid' : 'fa-light'} {icon} fa-fw" />
    {/if}
</button>
