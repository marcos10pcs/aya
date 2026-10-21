<script>
    import Button from "@/Controls/Button.svelte"
    import InputLabel from "@/Controls/InputLabel.svelte"
    import TextInput from "@/Controls/TextInput.svelte"
    import InputError from "@/Controls/InputError.svelte"
    import Card from "@/Components/Card.svelte"
    import Dropdown from "@/Controls/Dropdown.svelte"
    import Select from "@/Controls/Select.svelte"

    export let transport

    let ipAddress = ""
    let username = "Player"
    let virtualVersion = "0"
    let error = ""
    let hostPort = "53640"

    let bodyColors = []
    let charApp = []

    transport.characterAppearanceJSON().then(function(value) {
        console.error("initial: " + value);

        let selectedAssets = JSON.parse(value);

        if (selectedAssets.hat) {
            selectedAssets.hat.forEach(function (assetId) {
                charApp.push(assetId);
            });
        }

        if (selectedAssets.normal) {
            for (const subCategory in selectedAssets.normal) {
                if (selectedAssets.normal.hasOwnProperty(subCategory)) {
                    const assetIds = selectedAssets.normal[subCategory];
                    assetIds.forEach(function (assetId) {
                        charApp.push(assetId);
                    });
                }
            }
        }

        console.error("charapp: " + JSON.stringify(charApp))
    });

    transport.getBodyColorJson().then(
        function(value) {
            bodyColors = JSON.parse(value);
        }
    )

    function showCurrentMap() {
        transport.currentMap().then(
        function(value) {
            if(value == "") {
                value = "No Map Selected";
            }
            let filename = value.replace(/^.*[\\/]/, '')
            document.getElementById('map-selected').innerHTML = "&lrm;" + filename + "&lrm;";
        }
        );
    }

    showCurrentMap();

    async function submit() {
        try {
            transport.setUserName(username)

            // alert(username)

            let virtualVersionHost = parseInt(virtualVersion)
            let forceVirtualVersion = false //$('#host-forced-vv').is(':checked')

            const baseUrl = await transport.getMasterServerURL();

            const userId = Math.floor(Math.random() * 65535)
            console.error("finalCharApp charapp: " + JSON.stringify(charApp))
            const finalCharApp = baseUrl + '/avatar-fetch/?userid=' + userId + '&json=' + encodeURIComponent(JSON.stringify(charApp)) + '&body=' + encodeURIComponent(JSON.stringify(bodyColors));

            const obj = {
                ClientPort: 0,
                MachineAddress: "127.0.0.1",
                ServerPort: hostPort,
                UserName: username,
                DisplayName: username,
                CharacterAppearance: finalCharApp,
                GameId: 1818,
                PlaceId: 1818,
                PingInterval: 20,
                UserId: userId,
                CreatorId: 1,
                MembershipType: "None",      
                SuperSafeChat: false,
                IsUnknownOrUnder13: false,
                CreatorTypeEnum: "User",
                VirtualVersion: virtualVersionHost,
                ChatStyle: "ClassicAndBubble"
            };
            
            if (obj.VirtualVersion == 4) // hack to show chatbar when on 2012
            {
                transport.enableChatBarWidget();
            }

            // error = JSON.stringify(obj, null, 2);
            transport.startRcc(JSON.stringify(obj), parseInt(hostPort), obj.VirtualVersion, forceVirtualVersion);
        } catch (err) {
            error = "Failed to connect to server: " + err.message
        }
    }
</script>

<div class="grid h-full grid-cols-2 gap-3">
    <div class="col-span-1 grid grid-rows-2 gap-3">
        <Card title="Host Server" class="row-span-1" icon="fa-globe">
            <form on:submit|preventDefault={submit}>
                <div>
                    <InputLabel for="username" value="Username" />
                    <TextInput placeholder="Player" class="mt-1" id="username" type="text" bind:value={username} required />
                </div>
                
                <div>
                    <InputLabel for="ip" value="Virtual Version" />
                    <Select bind:value={virtualVersion}>
                        <option value="0">2016</option>
                        <option value="1">2015</option>
                        <option value="2">2014</option>
                        <option value="3">2013</option>
                        <option value="4">2012</option>
                    </Select>
                </div>

                <div>
                    <InputLabel for="port" value="Server Port" />
                    <TextInput placeholder="53640" class="mt-1" id="port" type="number" bind:value={hostPort} required />
                </div>

                <div>
                    <InputLabel for="file" value="Place File" /><br>
                    <Button class="mt-1 ksk-btn-sm !bg-neutral-200 !border-neutral-300 !text-neutral-500" type="button" text="Select File" icon="fa-file" id="file" on:click={() => { transport.openMapPickerDialog(); }} />
                </div>

                {#if error}
                    <InputError message={error} />
                {/if}

                <div class="mt-2 flex w-full justify-end">
                    <Button class="ksk-btn-sm" type="submit" text="Host" icon="fa-server" title="Host Server" />
                </div>
            </form>
        </Card>
        <Card title="Server Information" class="row-span-1" icon="fa-info">

        </Card>
    </div>
    <Card title="Choose Level" class="col-span-1" icon="fa-tree-city">placeholder</Card>
</div>
