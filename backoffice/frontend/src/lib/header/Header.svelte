<script lang="ts">
	import Fa from 'svelte-fa'
	import { faCog, faArrowRight, faSearch, faDoorOpen } from '@fortawesome/free-solid-svg-icons'
    import auth from "../auth/authService"
    import { onMount } from 'svelte';

    let isAuthenticated: boolean = false;
    let user: object;
    let logout;

    let goToHome = () => window.location = "/";
    let createKey = () => window.location = "/items/create";
    let openDoor = () => window.location = "/forceOpen";


    onMount(async ()=>{
        isAuthenticated = await auth.isAuthenticated();
        if(isAuthenticated) user = await auth.getUser();


        logout = async () => {
            await auth.logout();
        }
    })

    export const prerender = false
</script>
<span id="menu" class="px-4 md:px-10 py-10 flex md:justify-between md:items-center flex-col md:flex-row gap-4">
    <span class="flex justify-between">
    <a on:click={goToHome} ><span class="flex items-center gap-3 cursor-pointer"><span class="text-xl tracking-widest font-light hidden md:inline">doora</span> <span class="font-bold p-2 border text-green-700 border-green-700 md:border-none md:bg-green-700 md:text-white rounded text-xl">backoffice</span></span></a>
    {#if isAuthenticated}

    <span class="flex md:hidden ">
        <span><img src={user?.picture} class="inline w-10 h-10 rounded-full mr-2"/><span class="hidden md:inline">{user?.nickname}</span></span>
        <a class="flex items-center gap-2 cursor-pointer" on:click={logout}>LOG OUT<Fa icon={faArrowRight}/></a>
    </span>
    {/if}
</span>
    <span class="flex gap-10 flex-col-reverse md:flex-row mt-5 md:mt-0">

            {#if isAuthenticated}
            <span class="flex md:items-center gap-4 flex-col md:flex-row">
                <a on:click={openDoor}  class="flex items-center gap-1 cursor-pointer"><Fa icon={faDoorOpen}/>OPEN DOOR</a>
                <a on:click={createKey} class="items-center p-3 md:p-2 bg-green-700 cursor-pointer text-white md:border md:bg-white md:text-green-700 md:border-green-700">+ ISSUE KEY</a>
            </span>


                <span class="hidden md:flex gap-6 ">
                    <span><img src={user?.picture} class="inline w-10 h-10 rounded-full mr-2"/><span class="hidden md:inline">{user?.nickname}</span></span>
                    <a class="flex items-center gap-2" on:click={logout}>LOG OUT<Fa icon={faArrowRight}/></a>
                </span>

            {/if}
        </span>
</span>

<style>
@media print {
    #menu{
        display: none;
    }
}

</style>