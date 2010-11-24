
$(document).ready(function () { 


	$('ul.imageRotator li:first').show();

	var rotator = { timeout: null,
									currIndex: 1, 
									maxIndex: 6,
									speed: 400,
									delay: 5000,
									callback: function () {
											$('ul.imageRotator li:nth-child('+this.currIndex+')').hide(this.speed);
											this.currIndex++;
											if (this.currIndex>this.maxIndex) {
													this.currIndex=1;
											}
											$('ul.imageRotator li:nth-child('+this.currIndex+')').show(this.speed);
											rotator.timeout = window.setTimeout(function () { rotator.callback(); }, rotator.delay);
																
									}, 
								};

	rotator.timeout = window.setTimeout(function () { rotator.callback(); }, rotator.delay); 

});
