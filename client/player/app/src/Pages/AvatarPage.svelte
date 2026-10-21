<script>
    import { onDestroy, onMount } from "svelte";
    import { writable } from "svelte/store";
    import Button from "@/Controls/Button.svelte";
    import PillButtons from "@/Controls/PillButtons.svelte";

    export let transport;

    let response = writable([]);
    let wearingAssets = {
        "hat": [], // 5 max
        "normal": { // 1 max each
            "shirt": [],
            "tshirt": [],
            "pants": [],
            "face": [],
        },
    };

    function toggleAssetWearIcon(assetId) {
        let assetElement = document.querySelector(`[data-asset-id="${assetId}"]`);

        if (assetElement instanceof HTMLElement) {
            let buttonElement = assetElement.querySelector('.aya-btn-xs');

            let wearingElement = assetElement.querySelector('.wearing');

            if (buttonElement && wearingElement) {
                if (wearingElement.style.display === 'block') {
                    buttonElement.style.display = 'none';
                } else {
                    buttonElement.style.display = 'block';
                }
            }
        }
    }

    function wearAllAssets() {
        wearingAssets.hat.forEach(function (assetId) {
            transport.wearAsset(assetId, 'hat');
        });

        for (const subCategory in wearingAssets.normal) {
            if (wearingAssets.normal.hasOwnProperty(subCategory)) {
                const assetIds = wearingAssets.normal[subCategory];
                assetIds.forEach(function (assetId) {
                    transport.wearAsset(assetId, subCategory);
                    toggleAssetWearIcon(assetId);
                });
            }
        }

        console.error("All assets have been worn:", wearingAssets);
    }

    transport.characterAppearanceJSON().then(function(value) {
        console.error("initial: " + value);

        let selectedAssets = JSON.parse(value);

        if (selectedAssets.hat) {
            selectedAssets.hat.forEach(function (assetId) {
                if (wearingAssets.hat.length < 5) {
                    wearingAssets.hat.push(assetId);
                }
            });
        }

        if (selectedAssets.normal) {
            for (const subCategory in selectedAssets.normal) {
                if (selectedAssets.normal.hasOwnProperty(subCategory)) {
                    const assetIds = selectedAssets.normal[subCategory];
                    assetIds.forEach(function (assetId) {
                        if (wearingAssets.normal[subCategory].length < 1) {
                            wearingAssets.normal[subCategory].push(assetId);
                        }
                    });
                }
            }
        }

        console.error("Updated wearingAssets:", JSON.stringify(wearingAssets));
        wearAllAssets();
    });

    onMount(() => {
        transport.startAvatarView();
        fetchAssets();
    });

    let buffer;

    async function fetchAssets(category = "") {
        try {
            const baseUrl = await transport.getMasterServerURL();
            const url = baseUrl + "/catalog-assets" + (category ? `?type=${category}` : "");

            console.error(url);
            const res = await fetch(url);
            const data = await res.json();

            buffer = Object.values(data); // check wearAsset bottom
            response.set(Object.values(data)); 
        } catch (error) {
            console.error("Error fetching assets:", error);
            response.set([]);
        }
    }

    function wearAsset(element, category) {
        const assetId = element.dataset.assetId;

        console.error("wearAsset " + assetId);
        console.error("assetId " + category);

        const assetIdInt = parseInt(assetId);

        if (category == "hat")
        {
            const index = wearingAssets.hat.indexOf(assetIdInt);

            if (index !== -1) {
                wearingAssets.hat.splice(index, 1);
            } else {
                wearingAssets.hat.push(assetIdInt);
            }

            // is 6, so we should cut out first asset
            // 5 is max, but user just wore new stuffs so.
            if (wearingAssets.hat.length == 4)
            {
                transport.wearAsset(wearingAssets.hat[0], category);
                wearingAssets.hat.shift()
            }
        }
        else
        {
            console.error(JSON.stringify(wearingAssets.normal[category]));

            if (wearingAssets.normal[category]) {
                const index = wearingAssets.normal[category].indexOf(assetIdInt);

                if (index !== -1) {
                    wearingAssets.normal[category].splice(index, 1);
                } else {
                    wearingAssets.normal[category].push(assetIdInt);
                }

                if (wearingAssets.normal[category].length == 2)
                {
                    console.error("is wearing more than one.");

                    transport.wearAsset(wearingAssets.normal[category][0], category);
                    wearingAssets.normal[category].shift()
                }
            } else {
                console.error("Invalid category:", category);
            }
        }

        toggleAssetWearIcon(assetId);
        transport.wearAsset(assetId, category);

        // let allItems = [...wearingAssets.hat, ...wearingAssets.normal.face, ...wearingAssets.normal.shirt, ...wearingAssets.normal.pants, ...wearingAssets.normal.tshirt];
        // console.error(JSON.stringify(allItems));
        console.error(JSON.stringify(wearingAssets))
        transport.setCharacterAppearanceJSON(JSON.stringify(wearingAssets))

        // force rerendering
        response.set(buffer); 
    }

    let activeClothingType = "hat"

    const clothingTypes = [
        { id: "hat", label: "Hat", icon: "fa-hat-wizard" },
        { id: "shirt", label: "Shirts", icon: "fa-shirt" },
        { id: "pants", label: "Pants", icon: "fa-socks" },
        { id: "tshirt", label: "T-Shirts", icon: "fa-image" },
        { id: "face", label: "Faces", icon: "fa-head-side" },
    ]

    const handleClothingTypeChange = (event) => {
        console.error("category changed to:", event.detail.id)
        fetchAssets(event.detail.id)
    }

    onDestroy(() => {
        transport.stopAvatarView()
    })
</script>

<div style="scrollbar-width: none;">
    <div style="height: 100vh;overflow-y: scroll;scrollbar-width: none;">
        <h1 class="text-3xl font-bold">Customize Character</h1>
        <p>Press R to reset view</p>

        <hr class="mt-1 mb-1">
        <PillButtons items={clothingTypes} bind:activeItem={activeClothingType} on:itemChange={handleClothingTypeChange} />
        <hr class="mt-1 mb-1">

        <div class="px-2 py-2 sm:col-span-3 md:px-3 lg:col-span-5 xl:col-span-6">
            <div class="grid grid-cols-3 md:grid-cols-4 lg:grid-cols-6 xl:grid-cols-8 2xl:grid-cols-9">
                {#each $response as asset}
                    <!-- svelte-ignore a11y-click-events-have-key-events -->
                    <!-- svelte-ignore a11y-no-static-element-interactions -->
                    <div
                        on:click={(event) => wearAsset(event.currentTarget, asset.assetType)} 
                        aria-label="Item"
                        class="aya-item justify-content-center relative flex w-full flex-col items-center justify-center justify-items-center p-2 transition-all duration-200"
                        data-asset-id={asset.assetId}
                        data-category={asset.assetType}>
                        <div class="flex w-full flex-col">
                            <div class="relative overflow-hidden">
                                {#if asset.assetType === 'hat' && wearingAssets.hat.includes(asset.assetId)}
                                <Button on:hover={() => {}} class="aya-btn-xs absolute right-[5%] top-[5%]" icon="fa-hat-wizard" style="display: block;" />
                                {:else if wearingAssets.normal[asset.assetType] && wearingAssets.normal[asset.assetType].includes(asset.assetId)}
                                <Button on:hover={() => {}} class="aya-btn-xs absolute right-[5%] top-[5%]" icon="fa-shirt" style="display: block;" />
                                {/if}
                    
                                <img src="{asset.assetThumbnail}" width="130" class="w-full rounded-lg bg-neutral-200 p-2 dark:bg-neutral-800" />
                                <span class="absolute bottom-0 left-0 wearing" style="display: {asset.wearing ? 'block' : 'none'}" />
                            </div>
                            <span class="mt-1 truncate text-lg font-semibold text-neutral-950 dark:text-white">{asset.name}</span>
                            <span class="text-sm"><span class="font-semibold text-aya-500"><i class="fa-duotone fa-comet fa-fw me-0.5 text-aya-500/70" />Free</span></span>
                        </div>
                    </div>
                {/each}
            </div>
        </div>
    </div>
</div>
