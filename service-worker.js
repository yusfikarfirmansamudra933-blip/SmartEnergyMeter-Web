const CACHE_NAME = "SmartEnergyMeter-v2";

const urls = [
    "/SmartEnergyMeter-Web/",
    "/SmartEnergyMeter-Web/index.html",
    "/SmartEnergyMeter-Web/style.css",
    "/SmartEnergyMeter-Web/script.js",
    "/SmartEnergyMeter-Web/manifest.json",
    "/SmartEnergyMeter-Web/icon-192.png",
    "/SmartEnergyMeter-Web/icon-512.png"
];

self.addEventListener("install", event => {

    event.waitUntil(

        caches.open(CACHE_NAME)
            .then(cache => {

                return cache.addAll(urls);

            })

    );

    self.skipWaiting();

});


self.addEventListener("activate", event => {

    event.waitUntil(

        caches.keys()
            .then(keys => {

                return Promise.all(

                    keys
                        .filter(key => key !== CACHE_NAME)
                        .map(key => caches.delete(key))

                );

            })

    );

    self.clients.claim();

});


self.addEventListener("fetch", event => {

    event.respondWith(

        caches.match(event.request)
            .then(response => {

                return response || fetch(event.request);

            })

    );

});