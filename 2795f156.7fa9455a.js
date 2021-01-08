(window.webpackJsonp=window.webpackJsonp||[]).push([[9],{100:function(e,t,r){"use strict";r.d(t,"a",(function(){return p})),r.d(t,"b",(function(){return u}));var o=r(0),a=r.n(o);function n(e,t,r){return t in e?Object.defineProperty(e,t,{value:r,enumerable:!0,configurable:!0,writable:!0}):e[t]=r,e}function b(e,t){var r=Object.keys(e);if(Object.getOwnPropertySymbols){var o=Object.getOwnPropertySymbols(e);t&&(o=o.filter((function(t){return Object.getOwnPropertyDescriptor(e,t).enumerable}))),r.push.apply(r,o)}return r}function l(e){for(var t=1;t<arguments.length;t++){var r=null!=arguments[t]?arguments[t]:{};t%2?b(Object(r),!0).forEach((function(t){n(e,t,r[t])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(r)):b(Object(r)).forEach((function(t){Object.defineProperty(e,t,Object.getOwnPropertyDescriptor(r,t))}))}return e}function c(e,t){if(null==e)return{};var r,o,a=function(e,t){if(null==e)return{};var r,o,a={},n=Object.keys(e);for(o=0;o<n.length;o++)r=n[o],t.indexOf(r)>=0||(a[r]=e[r]);return a}(e,t);if(Object.getOwnPropertySymbols){var n=Object.getOwnPropertySymbols(e);for(o=0;o<n.length;o++)r=n[o],t.indexOf(r)>=0||Object.prototype.propertyIsEnumerable.call(e,r)&&(a[r]=e[r])}return a}var s=a.a.createContext({}),i=function(e){var t=a.a.useContext(s),r=t;return e&&(r="function"==typeof e?e(t):l(l({},t),e)),r},p=function(e){var t=i(e.components);return a.a.createElement(s.Provider,{value:t},e.children)},d={inlineCode:"code",wrapper:function(e){var t=e.children;return a.a.createElement(a.a.Fragment,{},t)}},m=a.a.forwardRef((function(e,t){var r=e.components,o=e.mdxType,n=e.originalType,b=e.parentName,s=c(e,["components","mdxType","originalType","parentName"]),p=i(r),m=o,u=p["".concat(b,".").concat(m)]||p[m]||d[m]||n;return r?a.a.createElement(u,l(l({ref:t},s),{},{components:r})):a.a.createElement(u,l({ref:t},s))}));function u(e,t){var r=arguments,o=t&&t.mdxType;if("string"==typeof e||o){var n=r.length,b=new Array(n);b[0]=m;var l={};for(var c in t)hasOwnProperty.call(t,c)&&(l[c]=t[c]);l.originalType=e,l.mdxType="string"==typeof e?e:o,b[1]=l;for(var s=2;s<n;s++)b[s]=r[s];return a.a.createElement.apply(null,b)}return a.a.createElement.apply(null,r)}m.displayName="MDXCreateElement"},65:function(e,t,r){"use strict";r.r(t),r.d(t,"frontMatter",(function(){return b})),r.d(t,"metadata",(function(){return l})),r.d(t,"rightToc",(function(){return c})),r.d(t,"default",(function(){return i}));var o=r(2),a=r(6),n=(r(0),r(100)),b={id:"object_module",title:"object",sidebar_label:"object",slug:"/modules/object_module"},l={unversionedId:"object_module",id:"object_module",isDocsHomePage:!1,title:"object",description:"This module groups together utility functions that can be used to manipulate",source:"@site/docs/object_module.md",slug:"/modules/object_module",permalink:"/melon/docs/modules/object_module",editUrl:"https://github.com/daniele-rapagnani/edit/master/docs/docs/object_module.md",version:"current",sidebar_label:"object",sidebar:"someSidebar",previous:{title:"number",permalink:"/melon/docs/modules/number_module"},next:{title:"path",permalink:"/melon/docs/modules/path_module"}},c=[{value:"Functions",id:"functions",children:[{value:"hasKey(obj, key)",id:"haskeyobj-key",children:[]},{value:"removeKey(obj, key)",id:"removekeyobj-key",children:[]},{value:"clone(obj, deep)",id:"cloneobj-deep",children:[]},{value:"merge(target, with, deep)",id:"mergetarget-with-deep",children:[]}]},{value:"Properties",id:"properties",children:[{value:"symbols.sumOperator",id:"symbolssumoperator",children:[]},{value:"symbols.subOperator",id:"symbolssuboperator",children:[]},{value:"symbols.mulOperator",id:"symbolsmuloperator",children:[]},{value:"symbols.divOperator",id:"symbolsdivoperator",children:[]},{value:"symbols.concatOperator",id:"symbolsconcatoperator",children:[]},{value:"symbols.compareOperator",id:"symbolscompareoperator",children:[]},{value:"symbols.getIndexOperator",id:"symbolsgetindexoperator",children:[]},{value:"symbols.setIndexOperator",id:"symbolssetindexoperator",children:[]},{value:"symbols.negOperator",id:"symbolsnegoperator",children:[]},{value:"symbols.sizeOperator",id:"symbolssizeoperator",children:[]},{value:"symbols.powOperator",id:"symbolspowoperator",children:[]},{value:"symbols.callOperator",id:"symbolscalloperator",children:[]},{value:"symbols.hashingFunction",id:"symbolshashingfunction",children:[]},{value:"symbols.iterator",id:"symbolsiterator",children:[]},{value:"symbols.nextFunction",id:"symbolsnextfunction",children:[]},{value:"symbols.getPropertyOperator",id:"symbolsgetpropertyoperator",children:[]},{value:"symbols.setPropertyOperator",id:"symbolssetpropertyoperator",children:[]}]}],s={rightToc:c};function i(e){var t=e.components,r=Object(a.a)(e,["components"]);return Object(n.b)("wrapper",Object(o.a)({},s,r,{components:t,mdxType:"MDXLayout"}),Object(n.b)("p",null,"This module groups together utility functions that can be used to manipulate\n",Object(n.b)("a",Object(o.a)({parentName:"p"},{href:"/melon/docs/types/object"}),Object(n.b)("inlineCode",{parentName:"a"},"Object"))," values.\nIt also exposes ",Object(n.b)("a",Object(o.a)({parentName:"p"},{href:"/melon/docs/types/symbol"}),Object(n.b)("inlineCode",{parentName:"a"},"Symbols"))," that can be used to customise an ",Object(n.b)("inlineCode",{parentName:"p"},"Object"),"'s\nbehavior."),Object(n.b)("h2",{id:"functions"},"Functions"),Object(n.b)("h3",{id:"haskeyobj-key"},"hasKey(obj, key)"),Object(n.b)("p",null,"Checks if the provided object has the requested key defined. This can be used to be sure that an object has a key even if the associated value is ",Object(n.b)("inlineCode",{parentName:"p"},"null"),".   "),Object(n.b)("table",null,Object(n.b)("thead",{parentName:"table"},Object(n.b)("tr",{parentName:"thead"},Object(n.b)("th",Object(o.a)({parentName:"tr"},{align:null}),"Argument"),Object(n.b)("th",Object(o.a)({parentName:"tr"},{align:null}),"Description"))),Object(n.b)("tbody",{parentName:"table"},Object(n.b)("tr",{parentName:"tbody"},Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),Object(n.b)("strong",{parentName:"td"},"obj")),Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),"The object holding the key")),Object(n.b)("tr",{parentName:"tbody"},Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),Object(n.b)("strong",{parentName:"td"},"key")),Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),"The key to check")))),Object(n.b)("p",null,Object(n.b)("strong",{parentName:"p"},"Returns:")," ",Object(n.b)("inlineCode",{parentName:"p"},"true")," if the key is present, ",Object(n.b)("inlineCode",{parentName:"p"},"false")," otherwise."),Object(n.b)("h3",{id:"removekeyobj-key"},"removeKey(obj, key)"),Object(n.b)("p",null,"Removes a given key from an ",Object(n.b)("inlineCode",{parentName:"p"},"Object"),". This may cause some internal re-allocation to be triggered and it's not the same as setting a key to ",Object(n.b)("inlineCode",{parentName:"p"},"null")," as the key will not appear in enumeration.   "),Object(n.b)("table",null,Object(n.b)("thead",{parentName:"table"},Object(n.b)("tr",{parentName:"thead"},Object(n.b)("th",Object(o.a)({parentName:"tr"},{align:null}),"Argument"),Object(n.b)("th",Object(o.a)({parentName:"tr"},{align:null}),"Description"))),Object(n.b)("tbody",{parentName:"table"},Object(n.b)("tr",{parentName:"tbody"},Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),Object(n.b)("strong",{parentName:"td"},"obj")),Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),"The object holding the key")),Object(n.b)("tr",{parentName:"tbody"},Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),Object(n.b)("strong",{parentName:"td"},"key")),Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),"The key to remove")))),Object(n.b)("p",null,Object(n.b)("strong",{parentName:"p"},"Returns:")," ",Object(n.b)("inlineCode",{parentName:"p"},"true")," if the key could be found, ",Object(n.b)("inlineCode",{parentName:"p"},"false")," otherwise."),Object(n.b)("h3",{id:"cloneobj-deep"},"clone(obj, ","[deep]",")"),Object(n.b)("p",null,"Clones an ",Object(n.b)("inlineCode",{parentName:"p"},"Object")," returning a copy of the original. When a deep clone is performed only objects will be cloned, values which are not objects but are managed by the GC (eg: arrays) will still point to the same value and will be reference assigned.   "),Object(n.b)("table",null,Object(n.b)("thead",{parentName:"table"},Object(n.b)("tr",{parentName:"thead"},Object(n.b)("th",Object(o.a)({parentName:"tr"},{align:null}),"Argument"),Object(n.b)("th",Object(o.a)({parentName:"tr"},{align:null}),"Description"))),Object(n.b)("tbody",{parentName:"table"},Object(n.b)("tr",{parentName:"tbody"},Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),Object(n.b)("strong",{parentName:"td"},"obj")),Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),"The object to be cloned")),Object(n.b)("tr",{parentName:"tbody"},Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),Object(n.b)("strong",{parentName:"td"},"deep")," ","\xa0"," ",Object(n.b)("sub",null,Object(n.b)("sup",null,"Optional"))),Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),Object(n.b)("inlineCode",{parentName:"td"},"true")," if it should be cloned recursively")))),Object(n.b)("p",null,Object(n.b)("strong",{parentName:"p"},"Returns:")," The cloned object."),Object(n.b)("h3",{id:"mergetarget-with-deep"},"merge(target, with, ","[deep]",")"),Object(n.b)("p",null,"Merges two objects adding to ",Object(n.b)("inlineCode",{parentName:"p"},"target")," any key that is missing from it but that was found in ",Object(n.b)("inlineCode",{parentName:"p"},"with"),".    "),Object(n.b)("table",null,Object(n.b)("thead",{parentName:"table"},Object(n.b)("tr",{parentName:"thead"},Object(n.b)("th",Object(o.a)({parentName:"tr"},{align:null}),"Argument"),Object(n.b)("th",Object(o.a)({parentName:"tr"},{align:null}),"Description"))),Object(n.b)("tbody",{parentName:"table"},Object(n.b)("tr",{parentName:"tbody"},Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),Object(n.b)("strong",{parentName:"td"},"target")),Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),"The object that will be modified")),Object(n.b)("tr",{parentName:"tbody"},Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),Object(n.b)("strong",{parentName:"td"},"with")),Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),"The object with the keys to add to ",Object(n.b)("inlineCode",{parentName:"td"},"target")," is any is missing")),Object(n.b)("tr",{parentName:"tbody"},Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),Object(n.b)("strong",{parentName:"td"},"deep")," ","\xa0"," ",Object(n.b)("sub",null,Object(n.b)("sup",null,"Optional"))),Object(n.b)("td",Object(o.a)({parentName:"tr"},{align:null}),Object(n.b)("inlineCode",{parentName:"td"},"true")," if the merging should be done recursively. Defaults to ",Object(n.b)("inlineCode",{parentName:"td"},"false"))))),Object(n.b)("p",null,Object(n.b)("strong",{parentName:"p"},"Returns:")," ",Object(n.b)("inlineCode",{parentName:"p"},"true")," if the two objects were merged successfuly, ",Object(n.b)("inlineCode",{parentName:"p"},"false")," otherwise."),Object(n.b)("h2",{id:"properties"},"Properties"),Object(n.b)("h3",{id:"symbolssumoperator"},"symbols.sumOperator"),Object(n.b)("p",null,"A symbol that can be used to overload the sum operator"),Object(n.b)("h3",{id:"symbolssuboperator"},"symbols.subOperator"),Object(n.b)("p",null,"A symbol that can be used to overload the subtraction operator"),Object(n.b)("h3",{id:"symbolsmuloperator"},"symbols.mulOperator"),Object(n.b)("p",null,"A symbol that can be used to overload the multiplication operator"),Object(n.b)("h3",{id:"symbolsdivoperator"},"symbols.divOperator"),Object(n.b)("p",null,"A symbol that can be used to overload the division operator"),Object(n.b)("h3",{id:"symbolsconcatoperator"},"symbols.concatOperator"),Object(n.b)("p",null,"A symbol that can be used to overload the concatenation operator"),Object(n.b)("h3",{id:"symbolscompareoperator"},"symbols.compareOperator"),Object(n.b)("p",null,"A symbol that can be used to provide custom comparison"),Object(n.b)("h3",{id:"symbolsgetindexoperator"},"symbols.getIndexOperator"),Object(n.b)("p",null,"A symbol that can be used to overload the indexed access operator"),Object(n.b)("h3",{id:"symbolssetindexoperator"},"symbols.setIndexOperator"),Object(n.b)("p",null,"A symbol that can be used to overload the indexed set operator"),Object(n.b)("h3",{id:"symbolsnegoperator"},"symbols.negOperator"),Object(n.b)("p",null,"A symbol that can be used to overload the negation operator"),Object(n.b)("h3",{id:"symbolssizeoperator"},"symbols.sizeOperator"),Object(n.b)("p",null,"A symbol that can be used to overload the size operator"),Object(n.b)("h3",{id:"symbolspowoperator"},"symbols.powOperator"),Object(n.b)("p",null,"A symbol that can be used to overload the power operator"),Object(n.b)("h3",{id:"symbolscalloperator"},"symbols.callOperator"),Object(n.b)("p",null,"A symbol that can be used to overload the call operator"),Object(n.b)("h3",{id:"symbolshashingfunction"},"symbols.hashingFunction"),Object(n.b)("p",null,"A symbol that can be used provide custom hashing mechanism"),Object(n.b)("h3",{id:"symbolsiterator"},"symbols.iterator"),Object(n.b)("p",null,"A symbol that can be used to overload the get iterator operator"),Object(n.b)("h3",{id:"symbolsnextfunction"},"symbols.nextFunction"),Object(n.b)("p",null,"A symbol that can be used to overload the next iterator","'","s value operator"),Object(n.b)("h3",{id:"symbolsgetpropertyoperator"},"symbols.getPropertyOperator"),Object(n.b)("p",null,"A symbol that can be used to overload the dot operator for property reading"),Object(n.b)("h3",{id:"symbolssetpropertyoperator"},"symbols.setPropertyOperator"),Object(n.b)("p",null,"A symbol that can be used to overload the dot operator for property writing"))}i.isMDXComponent=!0}}]);