<script>
  import Card from "@/Components/Card.svelte"
  import Button from "@/Controls/Button.svelte";
  import InputError from "@/Controls/InputError.svelte"

  let transport
  let servers = [];
  let error = "";

  async function refresh() {
    const rawJson = await transport.grabServers();
    const response = JSON.parse(rawJson);
    servers = response.ActiveServers;
  }
  
  if (process.env.NODE_ENV !== "development") {
    new QWebChannel(qt.webChannelTransport, (channel) => {
      transport = channel.objects.jsHelpers
      refresh();
    })
  }

  async function joinServer(i) {
    try {
      
      const baseUrl = transport.getMasterServerURL()
      const charApp = []
      const finalCharApp = `${baseUrl}/avatar-fetch/?json=${encodeURIComponent(JSON.stringify(charApp))}`

      alert(i);
      
      const connectionObj = {
        ClientPort: 0,
        MachineAddress: servers[i].machine_address,
        ServerPort: Number(servers[i].server_port),
        UserName: "Player",
        DisplayName: "Player",
        CharacterAppearance: finalCharApp,
        GameId: 1818,
        PlaceId: 1818,
        PingInterval: 20,
        UserId: Math.floor(Math.random() * 65535),
        CreatorId: 1,
        MembershipType: "None",
        SuperSafeChat: false,
        IsUnknownOrUnder13: false,
        CreatorTypeEnum: "User",
        ChatStyle: "ClassicAndBubble",
        VirtualVersion: 0,
        IsRobloxPlace: true
      }

      if (connectionObj.VirtualVersion === 4) {
        transport.enableChatBarWidget()
      }

      await transport.launchGame(JSON.stringify(connectionObj), connectionObj.VirtualVersion)            } catch (err) {
        error = "Failed to connect to server: " + err.message
      }
  }
</script>

<Card class="col-span-1 h-full" icon="fa-server">
  <svelte:fragment slot="title">
    <div class="flex w-full select-none items-center rounded-t-md border-b border-slate-200 bg-gray-100 px-2 py-1 font-semibold dark:border-neutral-700 dark:bg-neutral-900 dark:text-neutral-50">
      <i class="fa-regular fa-light fa-fw fa-server me-1" />
      <span>Server Browser</span>

      <Button text="Refresh" class="ms-1 ksk-btn-sm" on:click={refresh} />
    </div>
  </svelte:fragment>

  {#if error}
    <InputError message={error} />
  {/if}
  
  {#each servers as server, i}
    <div class="flex w-full items-center rounded-md border border-gray-200 bg-gray-100 px-3 py-1.5">
      <div class="w-full">
        <span class="text-gray-600">({server.player_count}/{server.player_limit})</span>
        <span class="font-semibold">{server.host}</span>'s {server.server_name} {i}
	<Button text="Join" class="ms-1 ksk-btn-sm" on:click={() => joinServer(i)} />
      </div>
      <!-- <img class="ms-1" height="10" src="./img/pbs.png" /> -->
      <!-- <img class="ms-1" height="10" src="./img/mega.png" /> --> 
    </div>
  {/each}
</Card>
