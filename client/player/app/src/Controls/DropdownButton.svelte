<script>
    export let shown = false
    export let onClick = () => {}
</script>

<button on:click={onClick} class="ksk-anim-pop rounded-lg text-neutral-500 transition duration-200 hover:text-neutral-600 {shown ? 'bg-black/10' : 'hover:bg-black/5'}">
    <slot {shown}>
        <i class="{shown ? 'fa-solid' : 'fa-regular'} fa-caret-down fa-fw ksk-anim-pop" />
    </slot>
</button>
