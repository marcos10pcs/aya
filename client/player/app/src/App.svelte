<script>
    import { onMount } from "svelte"
    import { spring } from "svelte/motion"

    import PageButton from "@/Components/PageButton.svelte"
    import PlayPage from "@/Pages/PlayPage.svelte"
    import HostPage from "@/Pages/HostPage.svelte"
    import AvatarPage from "@/Pages/AvatarPage.svelte"
    import LevelsPage from "@/Pages/LevelsPage.svelte"
    import ExtensionsPage from "@/Pages/ExtensionsPage.svelte"
    import PackagesPage from "@/Pages/PackagesPage.svelte"
    import FavoritesPage from "@/Pages/FavoritesPage.svelte"
    import SettingsPage from "@/Pages/SettingsPage.svelte"
    import AboutPage from "@/Pages/AboutPage.svelte"

    var transport = null
    if (process.env.NODE_ENV !== "development") {
        onMount(() => {
            new QWebChannel(qt.webChannelTransport, (channel) => {
                transport = channel.objects.jsHelpers
            })
        })
    }

    let selectedPage = "play"

    const pages = [
        { id: "play", title: "Play" },
        { id: "host", title: "Host" },
        { id: "avatar", title: "Character" },
        { id: "levels", title: "Levels" },
        { id: "extensions", title: "Extensions" },
        { id: "packages", title: "Packages" },
        { id: "favorites", title: "Favorites" },
        { id: "studio", title: "Open Aya Studio" },
        { id: "settings", title: "Settings", icon: "fa-cog" },
        { id: "about", title: "About", icon: "fa-circle-info" }
    ]

    let pageIndicator = spring(9.5, {
        stiffness: 0.15,
        damping: 0.7
    })

    $: if (selectedPage) {
        const activeButton = document.querySelector(`[data-page="${selectedPage}"]`)
        if (activeButton) {
            pageIndicator.set(activeButton.offsetTop + 2)
        }
    }
</script>

<div class="flex h-screen">
    <div class="flex w-[3.5rem] flex-col items-center border-r border-gray-300 bg-gray-100 pb-0.5 pt-2 text-white dark:border-neutral-950 dark:bg-neutral-900">
        <div class="absolute left-0 h-6 w-[0.175rem] rounded-r bg-ksk-500" style="transform: translateY({$pageIndicator}px); transition: height 0.2s ease" role="presentation" />
        {#each pages as page}
            {#if page.id == "studio"}
                <div class="flex-grow" />
                <PageButton {page} {selectedPage} onClick={() => transport.launchStudio()} />
            {:else}
                <PageButton {page} {selectedPage} onClick={() => (selectedPage = page.id)} />
            {/if}
        {/each}
    </div>

    <main class="flex-1 p-3 dark:bg-stone-950 dark:text-neutral-100 {selectedPage === 'avatar' ? 'border-r border-gray-300' : ''}">
        {#if selectedPage === "play"}
            <PlayPage {transport} />
        {:else if selectedPage === "host"}
            <HostPage {transport} />
        {:else if selectedPage === "avatar"}
            <AvatarPage {transport} />
        {:else if selectedPage === "levels"}
            <LevelsPage {transport} />
        {:else if selectedPage === "extensions"}
            <ExtensionsPage {transport} />
        {:else if selectedPage === "packages"}
            <PackagesPage {transport} />
        {:else if selectedPage === "favorites"}
            <FavoritesPage />
        {:else if selectedPage === "settings"}
            <SettingsPage {transport} />
        {:else if selectedPage === "about"}
            <AboutPage />
        {/if}
    </main>
</div>
