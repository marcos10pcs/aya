<script>
    import Button from "@/Controls/Button.svelte";

    export let currentPage;
    export let lastPage;
    export let routeName;

    const pageNumbers = () => {
        let pages = [];
        const maxPages = 10;
        let start = Math.max(currentPage - Math.floor(maxPages / 2), 1);
        let end = Math.min(start + maxPages - 1, lastPage);

        if (end - start < maxPages - 1) {
            start = Math.max(end - maxPages + 1, 1);
        }

        for (let i = start; i <= end; i++) {
            pages.push(i);
        }

        return pages;
    };

    const goToPage = (pageNumber) => {
        if (pageNumber < 1 || pageNumber > lastPage) return;
        window.location.href = window.route(routeName, { page: pageNumber });
    };
</script>

{#if lastPage > 1}
    <div class="mt-4 flex items-center justify-center space-x-0">
        <Button class="ksk-btn-sm border rounded-r-none rounded-l-md hover:bg-gray-100 dark:border-gray-600 dark:hover:bg-gray-700" text="‹" disabled={currentPage === 1} on:click={() => goToPage(currentPage - 1)}/>
        {#each pageNumbers() as pageNum}
            <Button class={`ksk-btn-sm border ${pageNum === currentPage ? 'text-gray-700' : 'hover:bg-gray-100'} ${pageNum === currentPage ? 'cursor-not-allowed' : ''} rounded-none dark:border-gray-600 dark:hover:bg-gray-700`} text={pageNum} disabled={pageNum === currentPage} on:click={() => pageNum !== currentPage && goToPage(pageNum)}/>
        {/each}
        <Button class="ksk-btn-sm border rounded-l-none rounded-r-md hover:bg-gray-100 dark:border-gray-600 dark:hover:bg-gray-700" text="›" disabled={currentPage === lastPage} on:click={() => goToPage(currentPage + 1)}/>
    </div>
{/if}
