<script context="module" lang="ts">
	export const prerender = false;
</script>

<script lang="ts">
	import Fa from 'svelte-fa'
	import { faSearch, faKey, faWifi } from '@fortawesome/free-solid-svg-icons'
	import API from "../lib/api"
	import Loading from "../lib/loading.svelte"
	import {onMount} from "svelte"
	// import auth from "../lib/auth/authService"
    import { page } from "$app/stores";
  import auth from '$lib/auth/authService';
  import Key from '../lib/entities/key';

	let isAuthenticated = false;
	let searchQuery = "";
	let items: any;
	let processing = false;
	let latestItems;
	let scanning = false;
	let detectedSN = "";

	let search = async () => {

	}

	let getKeyBySerialNumber = async () => {
        if ('NDEFReader' in window){
            //@ts-ignore
            const ndef = new NDEFReader();

            ndef.scan().then(() => {
                scanning = true;

                ndef.onreadingerror = () => {
                    alert("Cannot read data from the NFC tag. Try another one?");
                    scanning = false;
                };
                ndef.onreading = async (event) => {
                    detectedSN = event.serialNumber

					let key = await Key.getBySerialNumber(detectedSN);

					if(!key){
						alert("This key was not found on the database");
						location.reload()
					}else{
						window.location = `/items?id=${key.id}`
					}
                };
            }).catch(error => {
                alert(`Error! Scan failed to start: ${error}.`);
				scanning = false;
            });
        }else{
            alert("This feature is only available in Chrome Android. Please switch your browser or input the S/N manually.")

        }
	}

	onMount(async ()=>{
        if(!(await auth.isAuthenticated())) window.location = "/login"

		processing = true;
        let token = await auth.getToken();

		let {data} = await API.get(`/keys`,
            {
                headers: {
                    'Authorization': 'Bearer ' + token
                }
            });
		latestItems = data;
		let getShelf = $page.query.get("shelf");
		if(getShelf){
			searchQuery = `:shelf=${getShelf}`;
			await search();
			return;
		}

		items = latestItems;
		isAuthenticated = await auth.isAuthenticated();
		processing = false;
	})

	function shorten(text,max) {
	return text && text.length > max ? text.slice(0,max).split(' ').slice(0, -1).join(' ') + " [...]" : text
	}

</script>

<svelte:head>
	<title>Home</title>
</svelte:head>

<section class="flex flex-col px-4 py-4">
	{#if !scanning}
		<div class="flex items-center justify-center border p-5 gap-4 rounded">
			<Fa icon={faSearch}/>
			<input type="text" on:keyup={search} bind:value={searchQuery} id="search" placeholder="Type to search or enter barcode" class="w-full"/>
		</div>
		<span on:click={getKeyBySerialNumber} class="cursor-pointer text-teal-700 flex items-center gap-2"><Fa icon={faWifi}/>Scan tag</span>

		{:else}

		<span><Fa icon={faSearch}/>Please approach the tag ...</span>
	{/if}

	<div>
		{#if processing}
		<Loading/>
		{/if}

		{#if !processing && items && items.length == 0}
		 <span class="flex justify-center items-center text-center pt-10">😧 No keys were found.</span>
		{/if}

		{#if !processing && items && items.length > 0}
			<div class="flex flex-col gap-2 mt-4" id="results">
				{#if items == latestItems}
				<span class="pt-4 pb-2 tracking-wider text-gray-400">ISSUED KEYS</span>
				{/if}
				{#each items as item}
					<a href={`/items?id=${item.id}`}><span class="p-3 bg-gray-50 rounded tracking-widest flex justify-between"><span class="inline md:hidden">{shorten(item.user.email,23)}</span><span class="hidden md:inline">{shorten(item.user.email,40)}</span><span class="flex items-center gap-2 text-sm md:text-md text-gray-600"><Fa icon={faKey}/>{item.id}</span></span></a>
				{/each}
			</div>
		{/if}

	</div>
</section>

<style>

</style>