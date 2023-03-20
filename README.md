# voiconv

Conversion tool used in [Sumopaint](https://paint.sumo.app/).
It's fast and gives good compression results.

We are using stb_image and stb_image_write for PNG encoding/decoding. We were frustrated the performance of the encoding and the size of the produced pngs. It was far from good. We decided to try something else
We found Qoi, very simple and nice image encoding algorithm with no additional hacks/libraries. Qoi alone is better than (stb_image) pngs so we decided to use it. 
Our base engine already had Igor Pavlov's LZMA encoding support and we decided to try how it works with qois. The result were surprisingly good. 
The idea is to first encode the image with qoi and then pack it with LZMA. In this process, the very fast qoi compression decreases the size of the data going to slower LZMA (from 30-60% approximately) which helps the performance compared to directly compressing the image with LZMA.
Resulted image's are most of the time much smaller than png's at least when using some simple png encoders. Even professional software and with very good compression settings, the results are comparable.


Credits to original qoi repository: [phoboslab/qoi](https://github.com/phoboslab/qoi)
and to Igor Pavlov's LZMA: [7ZIP SDK](https://7-zip.org/sdk.html)


Compile (requires Make)
```sh
make
```

Example: Convert png to qoi
```sh
./voiconv foobar.png foobar.qoi
```

Example: Convert png to voi
```sh
./voiconv foobar.png foobar.voi
```

Example: Convert voi to png
```sh
./voiconv foobar.voi foobar.png
```

Some test results:
| File 					| ORIGINAL (png) 	| QOI		| VOI		| stb_image_png |
| -------------------------------	| --------------------	| -------------	| -----------	| ----------- 	|
| png-clipart-minecraft-minecraft.png	| 22.1kB		| 97.7kB	| 20.0kB	| 70.5kB	|
| bellard2.png				| 41.4kB		| 68.3kB	| 36.6kB	| 63.2kB	|
| Beer-PNG-10.png			| 285.8kB		| 370.3kB	| 131.1kB	| 339.4kB	|

Beer-PNG-10.png
![Beer-PNG-10](https://user-images.githubusercontent.com/24604650/226320633-ae4b943a-635d-4f27-b131-ca9740aa1413.png)
bellard2.png
![bellard2](https://user-images.githubusercontent.com/24604650/226320646-482947a7-9a0b-4def-a288-ec5a381aa4c1.png)
png-clipart-minecraft-minecraft.png
![png-clipart-minecraft-minecraft](https://user-images.githubusercontent.com/24604650/226320657-443c9e4e-f1c2-407b-810c-7411ab38187d.png)
