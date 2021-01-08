(window.webpackJsonp=window.webpackJsonp||[]).push([[29],{100:function(e,t,n){"use strict";n.d(t,"a",(function(){return p})),n.d(t,"b",(function(){return d}));var r=n(0),a=n.n(r);function o(e,t,n){return t in e?Object.defineProperty(e,t,{value:n,enumerable:!0,configurable:!0,writable:!0}):e[t]=n,e}function i(e,t){var n=Object.keys(e);if(Object.getOwnPropertySymbols){var r=Object.getOwnPropertySymbols(e);t&&(r=r.filter((function(t){return Object.getOwnPropertyDescriptor(e,t).enumerable}))),n.push.apply(n,r)}return n}function l(e){for(var t=1;t<arguments.length;t++){var n=null!=arguments[t]?arguments[t]:{};t%2?i(Object(n),!0).forEach((function(t){o(e,t,n[t])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(n)):i(Object(n)).forEach((function(t){Object.defineProperty(e,t,Object.getOwnPropertyDescriptor(n,t))}))}return e}function c(e,t){if(null==e)return{};var n,r,a=function(e,t){if(null==e)return{};var n,r,a={},o=Object.keys(e);for(r=0;r<o.length;r++)n=o[r],t.indexOf(n)>=0||(a[n]=e[n]);return a}(e,t);if(Object.getOwnPropertySymbols){var o=Object.getOwnPropertySymbols(e);for(r=0;r<o.length;r++)n=o[r],t.indexOf(n)>=0||Object.prototype.propertyIsEnumerable.call(e,n)&&(a[n]=e[n])}return a}var u=a.a.createContext({}),s=function(e){var t=a.a.useContext(u),n=t;return e&&(n="function"==typeof e?e(t):l(l({},t),e)),n},p=function(e){var t=s(e.components);return a.a.createElement(u.Provider,{value:t},e.children)},b={inlineCode:"code",wrapper:function(e){var t=e.children;return a.a.createElement(a.a.Fragment,{},t)}},m=a.a.forwardRef((function(e,t){var n=e.components,r=e.mdxType,o=e.originalType,i=e.parentName,u=c(e,["components","mdxType","originalType","parentName"]),p=s(n),m=r,d=p["".concat(i,".").concat(m)]||p[m]||b[m]||o;return n?a.a.createElement(d,l(l({ref:t},u),{},{components:n})):a.a.createElement(d,l({ref:t},u))}));function d(e,t){var n=arguments,r=t&&t.mdxType;if("string"==typeof e||r){var o=n.length,i=new Array(o);i[0]=m;var l={};for(var c in t)hasOwnProperty.call(t,c)&&(l[c]=t[c]);l.originalType=e,l.mdxType="string"==typeof e?e:r,i[1]=l;for(var u=2;u<o;u++)i[u]=n[u];return a.a.createElement.apply(null,i)}return a.a.createElement.apply(null,n)}m.displayName="MDXCreateElement"},85:function(e,t,n){"use strict";n.r(t),n.d(t,"frontMatter",(function(){return i})),n.d(t,"metadata",(function(){return l})),n.d(t,"rightToc",(function(){return c})),n.d(t,"default",(function(){return s}));var r=n(2),a=n(6),o=(n(0),n(100)),i={id:"introduction",title:"Introduction",sidebar_label:"Introduction",slug:"/"},l={unversionedId:"introduction",id:"introduction",isDocsHomePage:!1,title:"Introduction",description:"Melon is a dynamically typed, interpreted programming language. It attempts to combine the best features of popular high-level languages in something that feels modern but is still small and light enough to be easy to embed in your project. It's inspired mainly by ES6 JavaScript, Lua, Wren, Python and Ruby.",source:"@site/docs/index.md",slug:"/",permalink:"/melon/docs/",editUrl:"https://github.com/daniele-rapagnani/edit/master/docs/docs/index.md",version:"current",sidebar_label:"Introduction",sidebar:"someSidebar",next:{title:"Variables",permalink:"/melon/docs/variables"}},c=[{value:"Features",id:"features",children:[]}],u={rightToc:c};function s(e){var t=e.components,n=Object(a.a)(e,["components"]);return Object(o.b)("wrapper",Object(r.a)({},u,n,{components:t,mdxType:"MDXLayout"}),Object(o.b)("p",null,"Melon is a dynamically typed, interpreted programming language. It attempts to combine the best features of popular high-level languages in something that feels modern but is still small and light enough to be easy to embed in your project. It's inspired mainly by ",Object(o.b)("em",{parentName:"p"},"ES6 JavaScript, Lua, Wren, Python and Ruby"),"."),Object(o.b)("p",null,"It looks like this:"),Object(o.b)("pre",null,Object(o.b)("code",Object(r.a)({parentName:"pre"},{className:"language-js"}),'let Language = {\n    create = |name|  => {\n        return { name = name } @ Language;\n    },\n    \n    getName = -> {\n        return this.name;\n    }\n};\n\nlet Melon = {\n    create = => {\n        return { } @ Melon;\n    },\n    \n    helloWorld = -> {\n        io.print("Hello world " .. this->getName() .. "!");\n    }\n} @ Language.create("Melon");\n\nlet melon = Melon.create();\nmelon->helloWorld();\n')),Object(o.b)("h2",{id:"features"},"Features"),Object(o.b)("ul",null,Object(o.b)("li",{parentName:"ul"},"A simple but functional ",Object(o.b)("strong",{parentName:"li"},"module system")),Object(o.b)("li",{parentName:"ul"},"Good for ",Object(o.b)("strong",{parentName:"li"},"soft real-time")," applications such as games: it sports an ",Object(o.b)("strong",{parentName:"li"},"incremental generational GC")),Object(o.b)("li",{parentName:"ul"},"Native ",Object(o.b)("strong",{parentName:"li"},"integer")," and ",Object(o.b)("strong",{parentName:"li"},"array")," types allow for faster code"),Object(o.b)("li",{parentName:"ul"},"Support for ",Object(o.b)("strong",{parentName:"li"},"bytecode")," generation, you don't need to distribute your source files"),Object(o.b)("li",{parentName:"ul"},"Full support for ",Object(o.b)("strong",{parentName:"li"},"operator overloading")," using ",Object(o.b)("strong",{parentName:"li"},"ES6-style symbols")),Object(o.b)("li",{parentName:"ul"},Object(o.b)("strong",{parentName:"li"},"Prototype based"),": doesn't enforce the OOP paradigm while still making it possible"),Object(o.b)("li",{parentName:"ul"},Object(o.b)("strong",{parentName:"li"},"Dynamic typing")," without implicit conversions: lower chance of unintended behaviour"),Object(o.b)("li",{parentName:"ul"},"Entirely written in ",Object(o.b)("strong",{parentName:"li"},"C"),", the number of dependencies can be counted on one hand"),Object(o.b)("li",{parentName:"ul"},"Extensive ",Object(o.b)("strong",{parentName:"li"},"unit tests")," for both the VM and language")))}s.isMDXComponent=!0}}]);