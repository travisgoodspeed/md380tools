all: hugo-theme-docdock
hugo-theme-docdock:
	#Clone the theme.
	git clone https://github.com/vjeantet/hugo-theme-docdock
	#Patch reveal.js to disable checks for mobile devices.
	cp reveal.js hugo-theme-docdock/static/revealjs/js/reveal.js
clean:
	rm -rf hugo-theme-docdock
