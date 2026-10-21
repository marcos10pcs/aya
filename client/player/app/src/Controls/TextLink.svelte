<script>
    export let href
    export let text
</script>

<button {href} class="ksk-anim-pop rounded-lg text-ksk-500 underline transition duration-200 hover:text-ksk-400 {$$props.class ?? ''}">
    {text}
</button>
