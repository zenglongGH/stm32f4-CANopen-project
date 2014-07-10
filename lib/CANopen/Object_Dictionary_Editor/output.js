/*******************************************************************************

	output.js - JavaScript code for CANopenNode device configurator

	Copyright (C) 2007  Janez Paternoster, Slovenia

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


	Author: janez.paternoster@siol.net

*******************************************************************************/

//global variables
	//g_openerWindow is assigned by editor window
	//g_project is assigned by editor window
	//fileCO_OD_H
		var CO_OD_H_info = [];
		var CO_OD_H_macros = [];
		var CO_OD_H_typedefs = [];
		var CO_OD_H_RAM = [];
		var CO_OD_H_EEPROM = [];
		var CO_OD_H_ROM = [];
		var CO_OD_H_aliases = [];
	//fileCO_OD_C
		var CO_OD_C_initRAM = [];
		var CO_OD_C_initEEPROM = [];
		var CO_OD_C_initROM = [];
		var CO_OD_C_records = [];
                //MCO
		var CO_OD_C_functions = [];
		var CO_OD_C_OD = [];
	//fileEDSspec
		var EDSspec_info = [];
		var EDSspec_optObj = ["SupportedObjects=xx"];
		var EDSspec_manufObj = ["SupportedObjects=xx"];
		var EDSspec_objList = [];
	//fileXDDspec
		var XMLspec_objList = <></>;
	//fileDOC
		var DOCidentity = <></>;
		var DOCfeaturesIndex = <></>;
		var DOCfeatures = <></>;
		var DOCobjDictCommIndex = <></>;
		var DOCobjDictComm = <></>;
		var DOCobjDictManufIndex = <></>;
		var DOCobjDictManuf = <></>;
		var DOCobjDictProfileIndex = <></>;
		var DOCobjDictProfile = <></>;

	//other
		var fileName;
		var ODfileNameReference;
		var errorMessages = [];
		//following object is used with advanced and useful feature of Object dictionary editor. It enables combining
		//multiple objects into 'c' array. For example PDO mapping or communication parameters. Another example is
		//implementation of multiple similar devices in one canopen device. First device has multiple objects from index
		//6000h to 6FFFh, next device from 7000h to 7FFFh with same variables, and so on. If objects are combined,
		//then array is generated from each object, so application programmer can have something like OD_variable[0] for
		//first device and OD_variable[1] for second device.
		//following object 'combinedObjects' is collection of objects (one record for one combined object with specific
		//index), which are combined together into array:
		//combinedObjects = {index1:{}, index2:{}, ...};
		//{} contains following information:
			//firstIndex - index of the first element in the 'c' array (as hex text)
			//br1 - first backet
			//br2 - last backet
			//curentCount - curent index of array member with braces
			//only in first array object:
				//count - number of array members with braces
				//name
				//objectType
				//subNumber
				//memoryType
				//dataType
				//accessType
				//PDOmapping
			   //defaultValueMemorySize
				//subDefaultValueMemorySize
		var combinedObjects = {};

function generateFiles(){
	if(typeof(g_project) != "xml"){
		alert("No project is found, closing window!");
		window.close();
		return;
	}

	//set title to fileName
	fileName = g_project.other.file.@fileName.toString();
	if (fileName[0] == '_')
	{
		ODfileNameReference = fileName;
		//ODfileNameReference[0] = '_';
	}
	else
	{
		ODfileNameReference = "";
	}
//	document.title = fileName + " output - CANopenNode";

	//output g_project directly
	g_openerWindow.source.postMessage('XML<?xml version=\"1.0\"?>\n' + g_project.toXMLString(), "*");

	//calculate the contents
	calculateDeviceIdentity();
	calculateFeatures();
	calculateObjects();

	//generate final outputs
	generateCO_OD_H();
	generateCO_OD_C();
	generateEDSspec();
	generateXMLspec();
	generateDOC();

	//Finish
	//remove 'please wait ...' text
//	document.getElementById("introNote").hidden = true;
	if(errorMessages.length){
		var err = "Output files are generated with errors or warnings:\n\n" + errorMessages.join("\n");
		//document.getElementById("errors").removeAttribute("hidden");
		//document.getElementById("errors").value = err;
		alert(err);
	}
	//else alert("All output files are generated successfully without errors or warnings.");
}


function saveFile(idBuf, strFile){
	const nsIFilePicker = Components.interfaces.nsIFilePicker;
	var dir =
		Components.classes["@mozilla.org/file/directory_service;1"].getService(Components.interfaces.nsIProperties).get("CurProcD",
		Components.interfaces.nsILocalFile);
	var filePicker = Components.classes["@mozilla.org/filepicker;1"].createInstance(nsIFilePicker);
	filePicker.displayDirectory = dir;
	filePicker.init(window, "Hello", nsIFilePicker.modeSave);
	filePicker.defaultString = strFile;
	filePicker.appendFilters(filePicker.filterAll);

	var res = filePicker.show();
	if (res == nsIFilePicker.returnOK || res == nsIFilePicker.returnReplace){
		var thefile = filePicker.file;
		var foStream = Components.classes["@mozilla.org/network/file-output-stream;1"]
								.createInstance(Components.interfaces.nsIFileOutputStream);
		foStream.init(thefile, 0x02 | 0x08 | 0x20, 0666, 0); // write, create, truncate
		foStream.write(document.getElementById(idBuf).value, document.getElementById(idBuf).value.length);
		foStream.close();
	}
}

function calculateDeviceIdentity(){
	var idt = g_project.other.DeviceIdentity;
	var vendorName = idt.vendorName.toString();
	var vendorID = idt.vendorID.toString();
	var vendorText = idt.vendorText;
	var productName = idt.productName.toString();
	var productFamily = idt.productFamily.toString();
	var productID = idt.productID.toString();
	var productText = idt.productText;
	var deviceFamily = idt.deviceFamily;
	var buildDate = idt.buildDate.toString();
	var specificationRevision = idt.specificationRevision.toString();
	var instanceName = idt.instanceName.toString();

	/*** Generate contents CO_OD - c source code ***/
	CO_OD_H_info.push("      VendorName:     "+vendorName);
	CO_OD_H_info.push("      VendorNumber:   "+(vendorID?vendorID:"0"));
	CO_OD_H_info.push("      ProductName:    "+productName);
	CO_OD_H_info.push("      ProductNumber:  "+(productID?productID:"0"));

	/*** Generate contents for EDS specification ***/
	EDSspec_info.push(
		"VendorName="+vendorName+"\n"+
		"VendorNumber="+(vendorID?vendorID:"0")+"\n"+
		"ProductName="+productName+"\n"+
		"ProductNumber="+(productID?productID:"0")+"\n"+
		"RevisionNumber=0\n"+
		"OrderCode=0\n"+
		"BaudRate_10=1\n"+
		"BaudRate_20=1\n"+
		"BaudRate_50=1\n"+
		"BaudRate_125=1\n"+
		"BaudRate_250=1\n"+
		"BaudRate_500=1\n"+
		"BaudRate_800=1\n"+
		"BaudRate_1000=1\n"+
		"SimpleBootUpMaster=0\n"+
		"SimpleBootUpSlave=1\n"+
		"Granularity=8\n"+
		"DynamicChannelsSupported=0\n"+
		"GroupMessaging=0\n"+
		"NrOfRXPDO="+g_project.features.feature.(@name=="RPDO").@value.toString()+"\n"+
		"NrOfTXPDO="+g_project.features.feature.(@name=="TPDO").@value.toString()+"\n"+
		"LSS_Supported=0\n"
	);

	/*** Generate contents for Documentation ***/
	if(vendorName) vendorName = <tr><th>Vendor name</th><td>{vendorName}</td></tr>;
	if(vendorID) vendorID = <tr><th>Vendor ID</th><td>{vendorID}</td></tr>;
	if(vendorText.length()) vendorText = <tr><th>Vendor Text</th><td>{extractLabels(vendorText)}</td></tr>;
	if(productName) productName = <tr><th>Product Name</th><td>{productName}</td></tr>;
	if(productFamily) productFamily = <tr><th>Product Family</th><td>{productFamily}</td></tr>;
	if(productID) productID = <tr><th>Product ID</th><td>{productID}</td></tr>;
	if(productText.length()) productText = <tr><th>Product Text</th><td>{extractLabels(productText)}</td></tr>;
	if(deviceFamily.length()) deviceFamily = <tr><th>Device Family</th><td>{extractLabels(deviceFamily)}</td></tr>;
	if(buildDate) buildDate = <tr><th>Build Date</th><td>{buildDate}</td></tr>;
	if(specificationRevision) specificationRevision = <tr><th>Specification Revision</th><td>{specificationRevision}</td></tr>;
	if(instanceName) instanceName = <tr><th>Instance Name</th><td>{instanceName}</td></tr>;

	var orderNumber = <></>;
	for each(var el in idt.orderNumber)
		orderNumber += <div>{el}</div>
	if(orderNumber.length()) orderNumber = <tr><th>Order Number</th><td>{orderNumber}</td></tr>;

	var version = <></>;
	for each(var el in idt.version)
		version += <div>Version Type={el.@versionType}, version={el.toString()}</div>
	if(version.length()) version = <tr><th>Version</th><td>{version}</td></tr>;

	DOCidentity =
		<table>
			{vendorName}
			{vendorID}
			{vendorText}
			{productName}
			{productFamily}
			{productID}
			{productText}
			{deviceFamily}
			{orderNumber}
			{version}
			{buildDate}
			{specificationRevision}
			{instanceName}
		</table>;
}

function calculateFeatures(){
	for each(var feature in g_project.features.feature){
		var name = feature.@name.toString();
		var macroName = name.replace(/\W/g, "_").toUpperCase();
		var value = feature.@value.toString();
		var valueVal = parseInt(value);

		/*** Generate contents CO_OD - c source code ***/
		var ao = [];
		if(value != "0"){
			for each(var obj in feature.associatedObject){
				var index = obj.@index.toString().toUpperCase();
				var indexMax = obj.@indexMax.toString();
				if(indexMax){
					var indexStep = obj.@indexStep.toString();
					var indexVal = parseInt(index, 16);
					var indexMaxVal = parseInt(indexMax, 16);
					var indexStepVal = parseInt(indexStep, 16);
					if(indexStepVal < 1) indexStepVal = 1;
					var nr = 0;
					var iStr;
					for(var i=indexVal; i<=indexMaxVal; i+=indexStepVal){
						iStr = i.toString(16).toUpperCase();
						ao.push(iStr);
						combinedObjects[iStr] = {};
						combinedObjects[iStr].firstIndex = index;
						combinedObjects[iStr].curentCount = "[" + nr.toString() + "]";
						combinedObjects[iStr].br1 = " ";
						combinedObjects[iStr].br2 = "";
						if(++nr >= valueVal)	break;
					}
					//the first member contains number of members
					combinedObjects[index].count = "[" + nr.toString() + "]";
					combinedObjects[index].br1 = "{";
					combinedObjects[iStr].br2 = "}";
				}
				else{
					ao.push(index);
				}
			}
		}
		if(ao.length > 16){
			ao.sort();
			ao = "//Associated objects from index " + ao[0] + " to " + ao[ao.length - 1] + ", count = " + ao.length.toString();
		}
		else if(ao.length > 0)
			ao = "//Associated objects: " + ao.join(", ");
		else
			ao = "";
		CO_OD_H_macros.push("   #define CO"+ODfileNameReference+"_NO_" + indent(macroName, 25) + indent(value.toString(), 4) + ao);

		/*** Generate contents for Documentation ***/
		if(value != "0"){
			var href = "feature_" + macroName;
			var objects = <></>;
			var table = <></>
			for each(var obj in feature.associatedObject){
				var index = obj.@index.toString();
				var indexMax = obj.@indexMax.toString();
				var indexStep = obj.@indexStep.toString();
				objects +=
					<tr>
						<td>{g_project.CANopenObjectList.CANopenObject.(@index==index).@name}</td>
						<td>{index}</td>
						<td>{indexMax}</td>
						<td>{indexStep}</td>
					</tr>;
			}
			if(objects.length()){
				table =
					<table>
						<tr>
							<th>Name</th>
							<th>Index</th>
							<th>Max Index</th>
							<th>Index step</th>
						</tr>
						{objects}
					</table>;
			}

			DOCfeaturesIndex += <li><a href={"#"+href}>{name}</a> (value = {value})</li>
			DOCfeatures +=
				<div id={href} class="elements">
					<h3>{name}, value = {value}</h3>
					<h4>Associated objects:</h4>
					{table}
					{extractLabels(feature)}
				</div>
		}
	}
}

function calculateObjects(){
	var accessFunctionNameArray = [];
	var OD_nameArray = [];
	var combinedInitializations = {};
	var combinedInitializationsRAM = [];
	var combinedInitializationsEEPROM = [];
	var combinedInitializationsROM = [];

   var buttons =
         <li>
            <input type="button" pf="butReadAll" value="Read All"/>
            <input type="button" pf="butReadRR" value="Read RAM"/>
            <input type="button" pf="butRead" value="Read marked"/>
            <input type="button" pf="butWrite" value="Write marked"/>
         </li>
   DOCobjDictCommIndex += buttons;
   DOCobjDictManufIndex += buttons;
   DOCobjDictProfileIndex += buttons;

	//MCO
	CO_OD_C_functions.push("UNSIGNED32 CO_ODF"+ODfileNameReference+"(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);\n");

	for each(var object in g_project.CANopenObjectList.CANopenObject){
		if(object.@disabled != "true"){
			var index = object.@index.toString().toUpperCase();
			var indexVal = parseInt("0x"+index);
			var name = object.@name.toString();
			var nameNoSpace = name2c_code(name);
			var objectType = object.@objectType.toString();
			var subNumber = object.@subNumber.toString();
			var subNumberVal = parseInt(subNumber);
			var memoryType = object.@memoryType.toString();
			var dataType = g_byteToHexString(parseInt(object.@dataType.toString(), 16));
			var accessType = object.@accessType.toString();
			var PDOmapping = object.@PDOmapping.toString();
			var TPDOdetectCOS = object.@TPDOdetectCOS.toString();
			var defaultValue = object.@defaultValue.toString();
			var actualValue = object.@actualValue.toString();
			var accessFunctionName = object.@accessFunctionName.toString();
			var accessFunctionPreCode = object.accessFunctionPreCode.toString();
			var accessFunctionPostCode = object.accessFunctionPostCode.toString();

			/*** SDO Server access function ***/
			var DOCaccessSDOoverride = <></>;
			if(accessFunctionName){ //external function
			    //MCO
				accessFunctionName = accessFunctionName;
				DOCaccessSDOoverride = <h4>{"Default SDO Server access to object is replaced by external function: \""+accessFunctionName+"\"."}</h4>;
				if(accessFunctionNameArray.indexOf(accessFunctionName)<0){ //prevent duplicate decalrations
					CO_OD_C_functions.push("UNSIGNED32 "+accessFunctionName+"(void*, UNSIGNED16, UNSIGNED8, UNSIGNED16*, UNSIGNED16, UNSIGNED8, void*, const void*);\n");
					accessFunctionNameArray.push(accessFunctionName);
				}
			}
			else if(accessFunctionPreCode || accessFunctionPostCode){
				//add two spaces to the beginning of the every line
				if(accessFunctionPreCode) accessFunctionPreCode = accessFunctionPreCode.replace(/^(.*)/gm, "  $1") + "\n";
				if(accessFunctionPostCode) accessFunctionPostCode = accessFunctionPostCode.replace(/^(.*)/gm, "  $1") + "\n";
				var func =
					"UNSIGNED32 CO_ODF_"+index+ODfileNameReference+"(void *object, UNSIGNED16 index, UNSIGNED8 subIndex, UNSIGNED16* pLength,\n" +
					"                       UNSIGNED16 attribute, UNSIGNED8 dir, void* dataBuff, const void* pData){\n" +
					"  UNSIGNED32 abortCode;\n" +
					accessFunctionPreCode +
					"  abortCode = CO_ODF(object, index, subIndex, pLength, attribute, dir, dataBuff, pData);\n" +
					accessFunctionPostCode +
					"  return abortCode;\n" +
					"}";
				DOCaccessSDOoverride =
					<div><h4>Default SDO Server access to object is overrode by user defined function:</h4>
						<pre>{func}</pre>
					</div>
				CO_OD_C_functions.push(func+"\n");
				accessFunctionName = "CO_ODF_"+index+ODfileNameReference;
			}
			else{
				accessFunctionName = "CO_ODF";
			}

			function errorCombined(err){errorMessages.push("Error in object " + index + ": This object is combined with object " +
																		combinedObjects[index].firstIndex + " into Array. " + err + " of both objects must be the same!");}

			/*** Generate contents CO_OD - Source code in C ***/
			if(combinedObjects[index]){
				if(combinedObjects[index].firstIndex == index){
					//the first 'c' array member
					//data, which must be the same in all combined objects
					combinedObjects[index].name = name;
					combinedObjects[index].objectType = objectType;
					combinedObjects[index].subNumber = subNumber;
					combinedObjects[index].memoryType = memoryType;
				}
				else{
					//verify, if all next objects have the same attributes as the first object
					var firstIndex = combinedObjects[index].firstIndex;
					if(combinedObjects[firstIndex].name != name) errorCombined("Name");
					if(combinedObjects[firstIndex].objectType != objectType) errorCombined("Object Type");
					if(combinedObjects[firstIndex].subNumber != subNumber) errorCombined("Sub Number");
					if(combinedObjects[firstIndex].memoryType != memoryType) errorCombined("Memory Type");
				}
			}
			else{
				//this object is not combined
				combinedObjects[index] = {};
				combinedObjects[index].firstIndex = "";
				combinedObjects[index].count = "";
				combinedObjects[index].curentCount = "";
				combinedObjects[index].br1 = " ";
				combinedObjects[index].br2 = "";
			}

			var firstIndex = combinedObjects[index].firstIndex;

			//verify if nameNoSpace is unique
			if(combinedObjects[index].firstIndex == "" || combinedObjects[index].firstIndex == index){
				if(OD_nameArray.indexOf(nameNoSpace)>=0)
					errorMessages.push("Error in object " + index + ": Duplicate Name!");
				else OD_nameArray.push(nameNoSpace);
			}

			var varDefinition, varInitialization;
			switch(objectType){
				case "7":	//Variable
					var defaultValueMemorySize = dataType2c_codeSize(dataType, defaultValue)
					if(combinedObjects[index].firstIndex == "" || combinedObjects[index].firstIndex == index){
						//normal object or first combined object
						if(combinedObjects[index].firstIndex == index){
							//only first combined object
							combinedObjects[index].dataType = dataType;
							combinedObjects[index].accessType = accessType;
							combinedObjects[index].PDOmapping = PDOmapping;
							combinedObjects[index].defaultValueMemorySize = defaultValueMemorySize;
						}

						//data type and optional array definition
						var dt = dataType2c_code(dataType, defaultValue, combinedObjects[index].count, index);
						var arr = "";
						var isArr = dt.indexOf("[");
						if(isArr >= 0){
							arr = dt.slice(isArr);
							dt = dt.slice(0, isArr);
						}
						var indexRange = "/*" + index + ((combinedObjects[index].firstIndex==index)?((combinedObjects[index].count+"   ").slice(0, 6)):"      ");
						//object definition
						varDefinition = indexRange + "*/ " + indent(dt, 15) + nameNoSpace + arr + ";"

						//alias for object
						CO_OD_H_aliases.push(indexRange.replace(/\s/g,"") + ", Data Type: " + dt + (arr?", Array"+arr:"") + " */");
						CO_OD_H_aliases.push("      #define OD"+ODfileNameReference+"_" + indent(nameNoSpace, 40) + "CO"+ODfileNameReference+"_OD_" + memoryType + "." + nameNoSpace);
					}
					else{
						//next combined objects
						if(combinedObjects[firstIndex].dataType != dataType) errorCombined("Data Type");
						if(combinedObjects[firstIndex].accessType != accessType) errorCombined("Access Type");
						if(combinedObjects[firstIndex].PDOmapping != PDOmapping) errorCombined("PDO Mapping");
						if(combinedObjects[firstIndex].defaultValueMemorySize != defaultValueMemorySize) errorCombined("Length of Default value");
					}
					//object initial value
					varInitialization = "/*" + index + "*/"+ combinedObjects[index].br1 + value2c_code(defaultValue, dataType, index) + combinedObjects[index].br2 + ",";
					//attribute for Object Dictionary
					var attribute = attribute4c_code(memoryType, dataType, accessType, PDOmapping, TPDOdetectCOS);
					//put object into Object Dictionary
					var arrayType = "";
					if(dataType=="09" || dataType=="0A"){
						arrayType = "[0]";
						if(combinedObjects[index].firstIndex == "" || combinedObjects[index].firstIndex == index)
							CO_OD_H_aliases.push("      #define ODL"+ODfileNameReference+"_" + indent(nameNoSpace+"_stringLength",39) + defaultValueMemorySize.replace(" ", ""));
					}
               var varPtr = "(void*)&CO"+ODfileNameReference+"_OD_"+memoryType+"."+nameNoSpace+combinedObjects[index].curentCount+arrayType;
               if (dataType=="0F") varPtr = "0";   //DOMAIN data type
					CO_OD_C_OD.push("{0x"+index+", 0x00, 0x"+g_byteToHexString(attribute)+", "+defaultValueMemorySize+", "+
                                    varPtr+"},");
//													indent(varPtr+", ", 63)+accessFunctionName+"},");
					if(combinedObjects[index].firstIndex == "" || combinedObjects[index].firstIndex == index) CO_OD_H_aliases.push("");
					break;
				case "8":	//Array
					if(combinedObjects[index].firstIndex == "" || combinedObjects[index].firstIndex == index){
						//normal object or first combined object
						if(combinedObjects[index].firstIndex == index){
							//only first combined object
							combinedObjects[index].dataType = dataType;
							combinedObjects[index].accessType = accessType;
							combinedObjects[index].PDOmapping = PDOmapping;
						}

						//data type and optional array definition
						var defVal = object.CANopenSubObject[1].@defaultValue.toString(); //needed only for case, when dataType is string
						var dt = dataType2c_code(dataType, defVal, combinedObjects[index].count + "[" + (subNumberVal-1).toString() + "]", index);
						var arr = "";
						var isArr = dt.indexOf("[");
						if(isArr >= 0){
							arr = dt.slice(isArr);
							dt = dt.slice(0, isArr);
						}
						var indexRange = "/*" + index + ((combinedObjects[index].firstIndex==index)?((combinedObjects[index].count+"   ").slice(0, 6)):"      ");
						//object definition
						varDefinition = indexRange + "*/ " + indent(dt, 15) + nameNoSpace + arr + ";"

						//alias for object
						CO_OD_H_aliases.push(indexRange.replace(/\s/g,"") + ", Data Type: " + dt + (arr?", Array"+arr:"") + " */");
						CO_OD_H_aliases.push("      #define OD"+ODfileNameReference+"_" + indent(nameNoSpace, 40) + "CO"+ODfileNameReference+"_OD_" + memoryType + "." + nameNoSpace);
						CO_OD_H_aliases.push("      #define ODL"+ODfileNameReference+"_" + indent(nameNoSpace+"_arrayLength", 39) + (subNumberVal-1).toString());
					}
					else{
						//next combined objects
						if(combinedObjects[firstIndex].dataType != dataType) errorCombined("Data Type");
						if(combinedObjects[firstIndex].accessType != accessType) errorCombined("Access Type");
						if(combinedObjects[firstIndex].PDOmapping != PDOmapping) errorCombined("PDO Mapping");
					}

					//subObjects
					var subAlias = [];
					var subInitials = [];
					var subDefaultValueMemorySize = " 0";
					for(var i=1; i<subNumberVal; i++){
						var subObject = object.CANopenSubObject[i];
						if(subObject.@subIndex.toString() != g_byteToHexString(i))
							errorMessages.push("Error in object " + index + ": Wrong SubObjects!");

						var subName = subObject.@name.toString();
						var subNameNoSpace = name2c_code(subName);
						var subDefaultValue = subObject.@defaultValue.toString();
						var ms = dataType2c_codeSize(dataType, subDefaultValue);
						if(parseInt(ms) > parseInt(subDefaultValueMemorySize)) subDefaultValueMemorySize = ms;

						//initial value
						subInitials.push(value2c_code(subDefaultValue, dataType, index+", "+g_byteToHexString(i)));

						subAlias.push(subNameNoSpace);
					}

					if(combinedObjects[index].firstIndex == index){	//first combined object
						combinedObjects[index].subDefaultValueMemorySize = subDefaultValueMemorySize;
					}
					else if(combinedObjects[index].firstIndex != ""){	//next combined objects
						if(combinedObjects[firstIndex].subDefaultValueMemorySize != subDefaultValueMemorySize) errorCombined("Default Value Memory Size");
					}

					//object initial value
					varInitialization = "/*" + index + "*/"+ combinedObjects[index].br1 + "{" + subInitials.join(", ") + "}" + combinedObjects[index].br2 + ",";

					//attribute for Object Dictionary
					var subAccessType = object.CANopenSubObject[0].@accessType.toString();
					var attribute = attribute4c_code(memoryType, dataType, subAccessType=="rw"?"rw":accessType, PDOmapping, TPDOdetectCOS);
					//put object into Object Dictionary
					var arrayType = "[0]";
					if(dataType=="09" || dataType=="0A"){
						arrayType += "[0]";
						if(combinedObjects[index].firstIndex == "" || combinedObjects[index].firstIndex == index)
							CO_OD_H_aliases.push("      #define ODL"+ODfileNameReference+"_" + indent(nameNoSpace+"_stringLength", 39) + subDefaultValueMemorySize.replace(" ", ""));
					}
               var varPtr = "(void*)&CO"+ODfileNameReference+"_OD_"+memoryType+"."+nameNoSpace+combinedObjects[index].curentCount+arrayType;
               if (dataType=="0F") varPtr = "0";   //DOMAIN data type
					CO_OD_C_OD.push("{0x"+index+", 0x"+g_byteToHexString(subNumberVal-1)+", 0x"+g_byteToHexString(attribute)+", "+subDefaultValueMemorySize+", "+
                                    varPtr+"},");
//													indent(varPtr+", ", 49)+accessFunctionName+"},");

					//make aliases for subObjects (for each array member)
					if(combinedObjects[index].firstIndex == "" || combinedObjects[index].firstIndex == index){
						var makeAliases = true;
						//first make sure, that names of subObjects inside object are all different
						for(var i=0; i<subAlias.length; i++)
							for(var j=0; j<i; j++)
								if(subAlias[i] == subAlias[j]) makeAliases = false;
						if(makeAliases)
							for(var i=0; i<subAlias.length; i++)
								CO_OD_H_aliases.push("      #define ODA"+ODfileNameReference+"_" + indent(nameNoSpace+"_"+subAlias[i], 39) + i.toString());
					}

					if(combinedObjects[index].firstIndex == "" || combinedObjects[index].firstIndex == index) CO_OD_H_aliases.push("");
					break;
				case "9":	//Record
					//subObjects
					var subNames = [];
					var subDefinitions = [];
					var subAttributes = [];
					var subStructure = [];
					var subInitials = [];
					for(var i=0; i<subNumberVal; i++){
						var subObject = object.CANopenSubObject[i];
						if(subObject.@subIndex.toString() != g_byteToHexString(i))
							errorMessages.push("Error in object " + index + ": Wrong SubObjects!");

						//name
						var subName = subObject.@name.toString();
						var subNameNoSpace = name2c_code(subName);
						for(var j=0; j<i; j++)
							if(subNameNoSpace == subNames[j])
								errorMessages.push("Error in object " + index + ": Each SubObject must have unique name!");
						subNames.push(subNameNoSpace);
						//data type
						var subDataType = subObject.@dataType.toString();
						var subAccessType = subObject.@accessType.toString();
						var subPDOmapping = subObject.@PDOmapping.toString();
						var subTPDOdetectCOS = subObject.@TPDOdetectCOS.toString();
						var subDefaultValue = subObject.@defaultValue.toString();
						var subDefaultValueMemorySize = dataType2c_codeSize(subDataType, subDefaultValue)
						var subAttribute = attribute4c_code(memoryType, subDataType, subAccessType, subPDOmapping, subTPDOdetectCOS);
						//data type string and optional array definition
						var dt = dataType2c_code(subDataType, subDefaultValue, "", index+", "+g_byteToHexString(i));
						var arr = "";
						var isArr = dt.indexOf("[");
						if(isArr >= 0){
							arr = dt.slice(isArr);
							dt = dt.slice(0, isArr);
						}
						//output
						subDefinitions.push("               " + indent(dt, 15) + subNameNoSpace + arr + ";");
						subAttributes.push(subAttribute);
                  var varPtr = "(void*)&CO"+ODfileNameReference+"_OD_"+memoryType+"."+nameNoSpace+combinedObjects[index].curentCount+"."+subNameNoSpace+(arr?"[0]":"");
                  if (subDataType=="0F") varPtr = "0";   //DOMAIN data type
						subStructure.push("           {"+varPtr+", 0x"+g_byteToHexString(subAttribute)+", "+subDefaultValueMemorySize+"}");
						subInitials.push(value2c_code(subDefaultValue, subDataType, index+", "+g_byteToHexString(i)));
					}

					if(combinedObjects[index].firstIndex == "" || combinedObjects[index].firstIndex == index){
						//normal object or first combined object
						if(combinedObjects[index].firstIndex == index){
							//only first combined object
							combinedObjects[index].dataType = subDefinitions.toString();
							combinedObjects[index].accessType = subAttributes.toString();
						}

						var indexRange = "/*" + index + ((combinedObjects[index].firstIndex==index)?((combinedObjects[index].count+"   ").slice(0, 6)):"      ");

						//typedef for record
						CO_OD_H_typedefs.push(indexRange + "*/ typedef struct{");
						CO_OD_H_typedefs.push(subDefinitions.join("\n"));
						CO_OD_H_typedefs.push("               }              OD"+ODfileNameReference+"_" + nameNoSpace + "_t;\n");

						//object definition
						varDefinition = indexRange + "*/ " + indent("OD"+ODfileNameReference+"_"+nameNoSpace+"_t", 15) + nameNoSpace + combinedObjects[index].count + ";"

						//alias for object
						CO_OD_H_aliases.push(indexRange.replace(/\s/g,"") + ", Data Type: " + "OD_"+nameNoSpace + "_t" + (combinedObjects[index].count?", Array"+combinedObjects[index].count:"") + " */");
						CO_OD_H_aliases.push("      #define OD"+ODfileNameReference+"_" + indent(nameNoSpace, 40) + "CO"+ODfileNameReference+"_OD_" + memoryType + "." + nameNoSpace);
					}
					else{
						//next combined objects
						if(combinedObjects[firstIndex].dataType != subDefinitions.toString()) errorCombined("Data Type on all SubObjects");
						if(combinedObjects[firstIndex].accessType != subAttributes.toString()) errorCombined("Access Type and PDO mapping on all SubObjects");
					}

					//object initial value
					varInitialization = "/*" + index + "*/"+ combinedObjects[index].br1 + "{" + subInitials.join(", ") + "}" + combinedObjects[index].br2 + ",";

					//Information obout record for object dictionary
					CO_OD_C_records.push("/*0x"+index+"*/ const CO_OD_entryRecord_t OD"+ODfileNameReference+"_record"+index+"["+subNumberVal+"] = {");
					CO_OD_C_records.push(subStructure.join(",\n") + "};");

					//put object into Object Dictionary
					CO_OD_C_OD.push("{0x"+index+", 0x"+g_byteToHexString(subNumberVal-1)+", 0x00,  0, (void*)&"+
													"OD"+ODfileNameReference+"_record"+index+"},");
//													indent("OD"+ODfileNameReference+"_record"+index+", ", 49)+accessFunctionName+"},");

					if(combinedObjects[index].firstIndex == "" || combinedObjects[index].firstIndex == index) CO_OD_H_aliases.push("");
					break;
			}
			switch(memoryType){
				case "RAM":
					if(combinedObjects[index].firstIndex == "" || combinedObjects[index].firstIndex == index){
						CO_OD_H_RAM.push(varDefinition);
						CO_OD_C_initRAM.push(varInitialization);
						if(combinedObjects[index].firstIndex == index){
							combinedInitializations[index] = CO_OD_C_initRAM.length;
							combinedInitializationsRAM.push(index);
						}
					}
					else{
						//put initialization on correct place
						CO_OD_C_initRAM.splice(combinedInitializations[firstIndex], 0, varInitialization);
						for(var i=0; i<combinedInitializationsRAM.length; i++){
							if(firstIndex <= combinedInitializationsRAM[i]){
								combinedInitializations[combinedInitializationsRAM[i]]++;
							}
						}
					}
					break;
				case "EEPROM":
					if(combinedObjects[index].firstIndex == "" || combinedObjects[index].firstIndex == index){
						CO_OD_H_EEPROM.push(varDefinition);
						CO_OD_C_initEEPROM.push(varInitialization);
						if(combinedObjects[index].firstIndex == index){
							combinedInitializations[index] = CO_OD_C_initEEPROM.length;
							combinedInitializationsEEPROM.push(index);
						}
					}
					else{
						//put initialization on correct place
						CO_OD_C_initEEPROM.splice(combinedInitializations[firstIndex], 0, varInitialization);
						for(var i=0; i<combinedInitializationsEEPROM.length; i++){
							if(firstIndex <= combinedInitializationsEEPROM[i]){
								combinedInitializations[combinedInitializationsEEPROM[i]]++;
							}
						}
					}
					break;
				case "ROM":
					if(combinedObjects[index].firstIndex == "" || combinedObjects[index].firstIndex == index){
						CO_OD_H_ROM.push(varDefinition);
						CO_OD_C_initROM.push(varInitialization);
						if(combinedObjects[index].firstIndex == index){
							combinedInitializations[index] = CO_OD_C_initROM.length;
							combinedInitializationsROM.push(index);
						}
					}
					else{
						//put initialization on correct place
						CO_OD_C_initROM.splice(combinedInitializations[firstIndex], 0, varInitialization);
						for(var i=0; i<combinedInitializationsROM.length; i++){
							if(firstIndex <= combinedInitializationsROM[i]){
								combinedInitializations[combinedInitializationsROM[i]]++;
							}
						}
					}
					break;
			}

			/*** Generate contents for EDS specification ***/
			var EDSdefaultValue = defaultValue;
			if(dataType=="0A") EDSdefaultValue = EDSdefaultValue.replace(/\s/g, "");
			if(0x2000 <=indexVal && indexVal <0x6000)
				EDSspec_manufObj.push(EDSspec_manufObj.length.toString()+"=0x"+index+"\n");
			else if(indexVal!=0x1000 && indexVal!=0x1001 && indexVal!=0x1018)
				EDSspec_optObj.push(EDSspec_optObj.length.toString()+"=0x"+index+"\n");
			var EDScontents = [];
			switch(objectType){
				case "7":	//Variable
					EDScontents.push(
						"["+index+"]"+
						"\nParameterName="+name+
						"\nObjectType=7"+
						"\nDataType=0x00"+dataType+
						"\nAccessType="+EDSaccessType(accessType, PDOmapping)+
						"\nPDOMapping="+(PDOmapping=="no"?"0":"1")+
						"\nDefaultValue="+EDSdefaultValue+"\n\n"
					);
					break;
				case "8":	//Array
				case "9":	//Record
					EDScontents.push(
						"["+index+"]"+
						"\nParameterName="+name+
						"\nObjectType="+objectType+
						"\nSubNumber="+subNumber+"\n\n"
					);
					for(var i=0; i<subNumberVal; i++){
						var subObject = object.CANopenSubObject[i];
						var subDataType = subObject.@dataType.toString();
						var subAccessType = EDSaccessType(subObject.@accessType, subObject.@PDOmapping);
						var subPDOmapping = (subObject.@PDOmapping.toString()=="no") ? "0" : "1";
						var subDefaultValue = subObject.@defaultValue;
						//subObjects in arrays have some common attributes
						if(objectType=="8" && subObject.@subIndex.toString() != "00"){
							subDataType = dataType;
							subAccessType = EDSaccessType(accessType, PDOmapping);
							subPDOmapping = (PDOmapping=="no") ? "0" : "1";
						}
						//remove spaces for octet string
						if(subDataType=="0A") subDefaultValue = subDefaultValue.replace(/\s/g, "");
						EDScontents.push(
							"["+index+"sub"+subObject.@subIndex.toString().replace(/^0/, "")+"]"+
							"\nParameterName="+subObject.@name+
							"\nObjectType=7"+
							"\nDataType=0x00"+subDataType+
							"\nAccessType="+subAccessType+
							"\nPDOMapping="+subPDOmapping+
							"\nDefaultValue="+subDefaultValue+"\n\n"
						);
					}
					break;
			}

			/*** Generate contents for XML specification ***/
			var XMLcontents;
			switch(objectType){
				case "7":	//Variable
					XMLcontents =
						<CANopenObject index={index} name={name} objectType={objectType} dataType={dataType} accessType={accessType} defaultValue={defaultValue} PDOmapping={PDOmapping}/>;
					break;
				case "8":	//Array
					var XMLcontSubidx = <></>
					for(var i=0; i<subNumberVal; i++){
						var subObject = object.CANopenSubObject[i];
						if(subObject.@subIndex.toString() == "00") XMLcontSubidx += subObject;
						else XMLcontSubidx += <CANopenSubObject subIndex={subObject.@subIndex} name={subObject.@name} objectType="7" dataType={dataType} accessType={accessType} defaultValue={subObject.@defaultValue} PDOmapping={PDOmapping}/>
					}
					XMLcontents = <CANopenObject index={index} name={name} objectType={objectType} subNumber={subNumber}>{XMLcontSubidx}</CANopenObject>;
					break;
				case "9":	//Record
					var XMLcontSubidx = <></>
					for(var i=0; i<subNumberVal; i++){
						var subObject = object.CANopenSubObject[i];
						XMLcontSubidx += <CANopenSubObject subIndex={subObject.@subIndex} name={subObject.@name} objectType="7" dataType={subObject.@dataType} accessType={subObject.@accessType} defaultValue={subObject.@defaultValue} PDOmapping={subObject.@PDOmapping}/>
					}
					XMLcontents = <CANopenObject index={index} name={name} objectType={objectType} subNumber={subNumber}>{XMLcontSubidx}</CANopenObject>;
			}

			/*** Generate contents for Documentation ***/
			var DOCcontents;
			var DOChref = fileName + "_" + index.toString().replace(/\W/g, "_");
			var DOCobjectIndex;
			switch(objectType){
				case "7":	//Variable
               var size = dataType2c_codeSize(dataType, defaultValue).replace(" ", "0");
               var format = dataType2pfFormat(dataType, defaultValue);
               var docAccType = accessType;
               if(docAccType == "const") docAccType = "ro";
               if(memoryType == "RAM" && docAccType.search("r") >= 0) docAccType = "r" + docAccType;
               DOCobjectIndex =
                  <li>
                     <a docref={DOChref}>{index}</a> - {name}:
                     <input addr={index+"00"+"00"+size}
                            format={format}
                            pf={docAccType}
                            value={defaultValue}/>
                  </li>
					DOCcontents =
						<table>
							<tr>
								<th>Object Type</th>
								<th>Memory Type</th>
								<th>Data Type</th>
								<th>Access Type</th>
								<th>PDO Mapping</th>
								<th>TPDO detects COS</th>
								<th>Default value</th>
								<th>Actual value</th>
							</tr>
							<tr>
								<td>Var</td>
								<td>{memoryType}</td>
								<td>{g_CANopenDataType(dataType).slice(5)}</td>
								<td>{accessType}</td>
								<td>{PDOmapping}</td>
								<td>{TPDOdetectCOS=="true"?"yes":"no"}</td>
								<td>{defaultValue}</td>
								<td>{actualValue}</td>
							</tr>
						</table>;
					break;
				case "8":	//Array
					var DOCcontSubidx = <></>
					var DOCobjectSubIdx = <></>
					for(var i=0; i<subNumberVal; i++){
						var subObject = object.CANopenSubObject[i];
						DOCcontSubidx +=
							<tr>
								<td>{subObject.@subIndex}</td>
								<td>{subObject.@name}</td>
								<td>{subObject.@defaultValue}</td>
								<td>{subObject.@actualValue}</td>
							</tr>
                  if(i==0){
                     var size = "01";
                     var format = "U08";
                  }
                  else{
                     var size = dataType2c_codeSize(dataType, subObject.@defaultValue).replace(" ", "0");
                     var format = dataType2pfFormat(dataType, subObject.@defaultValue);
                  }
                  var docAccType = accessType;
                  if(docAccType == "const") docAccType = "ro";
                  if(memoryType == "RAM" && docAccType.search("r") >= 0) docAccType = "r" + docAccType;
                  DOCobjectSubIdx +=
                     <li>{subObject.@subIndex} - {subObject.@name}:
                        <input addr={index+subObject.@subIndex+"00"+size}
                               format={format}
                               pf={docAccType}
                               value={subObject.@defaultValue}/>
                     </li>
					}
               DOCobjectIndex = <li class="expandable"><a docref={DOChref}>{index} - {name}</a><ul>{DOCobjectSubIdx}</ul></li>
					DOCcontents =
						<table>
							<tr>
								<th>Object Type</th>
								<th>Sub Number</th>
								<th>Memory Type</th>
								<th>Data Type</th>
								<th>Access Type</th>
								<th>PDO Mapping</th>
								<th>TPDO detects COS</th>
							</tr>
							<tr>
								<td>Array</td>
								<td>{subNumber}</td>
								<td>{memoryType}</td>
								<td>{g_CANopenDataType(dataType).slice(5)}</td>
								<td>{accessType}</td>
								<td>{PDOmapping}</td>
								<td>{TPDOdetectCOS=="true"?"yes":"no"}</td>
							</tr>
						</table>+
						<br/>+
						<table>
							<tr>
								<th>Subindex</th>
								<th>Name</th>
								<th>Default value</th>
								<th>Actual value</th>
							</tr>
							{DOCcontSubidx}
						</table>;
					break;
				case "9":	//Record
					var DOCcontSubidx = <></>
					var DOCobjectSubIdx = <></>
					for(var i=0; i<subNumberVal; i++){
						var subObject = object.CANopenSubObject[i];
                  if(subObject.@disabled == "true") continue;

						DOCcontSubidx +=
							<tr>
								<td>{subObject.@subIndex}</td>
								<td>{subObject.@name}</td>
								<td>{g_CANopenDataType(subObject.@dataType.toString()).slice(5)}</td>
								<td>{subObject.@accessType}</td>
								<td>{subObject.@PDOmapping}</td>
								<td>{subObject.@TPDOdetectCOS=="true"?"yes":"no"}</td>
								<td>{subObject.@defaultValue}</td>
								<td>{subObject.@actualValue}</td>
							</tr>
                  var size = dataType2c_codeSize(subObject.@dataType.toString(), subObject.@defaultValue).replace(" ", "0");
                  var format = dataType2pfFormat(subObject.@dataType.toString(), subObject.@defaultValue);
                  var docAccType = subObject.@accessType;
                  if(docAccType == "const") docAccType = "ro";
                  if(memoryType == "RAM" && docAccType.search("r") >= 0) docAccType = "r" + docAccType;
                  DOCobjectSubIdx +=
                     <li>{subObject.@subIndex} - {subObject.@name}:
                        <input addr={index+subObject.@subIndex+"00"+size}
                               format={format}
                               pf={docAccType}
                               value={subObject.@defaultValue}/>
                     </li>
					}
               DOCobjectIndex = <li class="expandable"><a docref={DOChref}>{index} - {name}</a><ul>{DOCobjectSubIdx}</ul></li>
					DOCcontents =
						<table>
							<tr>
								<th>Object Type</th>
								<th>Sub Number</th>
								<th>Memory Type</th>
							</tr>
							<tr>
								<td>Record</td>
								<td>{subNumber}</td>
								<td>{memoryType}</td>
							</tr>
						</table>+
						<br/>+
						<table>
							<tr>
								<th>Subindex</th>
								<th>Name</th>
								<th>Data Type</th>
								<th>Access Type</th>
								<th>PDO Mapping</th>
								<th>TPDO detects COS</th>
								<th>Default value</th>
								<th>Actual value</th>
							</tr>
							{DOCcontSubidx}
						</table>;
					break;
			}

			/*** Output ***/
			//EDS specification
			EDSspec_objList.push(EDScontents.join(""));

			//XML specification
			XMLspec_objList += XMLcontents;

			//Documentation
			var DOCobject =
				<li id={DOChref}>
					<h3>{index} - {name}</h3>
					{DOCcontents}
					{extractLabels(object)}
				</li>

			if(indexVal < 0x2000){
				DOCobjDictCommIndex += DOCobjectIndex;
				DOCobjDictComm += DOCobject
			}
			else if(indexVal < 0x6000){
				DOCobjDictManufIndex += DOCobjectIndex;
				DOCobjDictManuf += DOCobject
			}
			else{
				DOCobjDictProfileIndex += DOCobjectIndex;
				DOCobjDictProfile += DOCobject
			}
		}
	}
}

function EDSaccessType(accessType, PDOmapping){
	if(accessType=="rw"){
		if(PDOmapping=="RPDO" || PDOmapping=="optional") return "rww";
		else if(PDOmapping=="TPDO") return "rwr";
	}
	return accessType;
}

function indent(string, length){
	if(string.length >= length) return string + " ";
	string += "                                                                                                    ";
	return string.slice(0, length);
}

function name2c_code(name){
	//remove all nonWord characters and make first word letters uppercase
	var arr = name.split(/\W+/);
	var str = arr[0].charAt(1);
	if("a"<=str && str<="z"){
		var str = arr[0];
		arr[0] = str.charAt(0).toLowerCase() + str.slice(1);
	}
	var prevLastChar = str.charAt(str.length-1);
	for(var i=1; i<arr.length; i++){
		var str = arr[i];
		arr[i] = str.charAt(0).toUpperCase() + str.slice(1);
		if(("A"<=prevLastChar && prevLastChar<="Z") && ("A"<=str.charAt(1) && str.charAt(1)<="Z")) arr[i] = "_" + arr[i];
		prevLastChar = str.charAt(str.length-1);
	}
	return arr.join("");
}

function dataType2c_code(dataType, value, arrayDef, object){
  dataType = g_CANopenDataType(dataType).slice(5);
	if(dataType == "VISIBLE_STRING")
		dataType += arrayDef + "["+value.length.toString()+"]";
	else if(dataType == "OCTET_STRING")
		dataType += arrayDef + "["+(value.replace(/\s/g, "").length/2).toString()+"]";
	else
		dataType += arrayDef;
	return dataType;
}

function dataType2c_codeSize(dataType, value){
	switch(dataType){
		case "02": //INTEGER8
		case "05": //UNSIGNED8
			return " 1";
		case "03": //INTEGER16
		case "06": //UNSIGNED16
			return " 2";
		case "10": //INTEGER24
		case "16": //UNSIGNED24
			return " 3";
		case "04": //INTEGER32
		case "07": //UNSIGNED32
		case "08": //REAL32
			return " 4";
		case "12": //INTEGER40
		case "18": //UNSIGNED40
			return " 5";
		case "13": //INTEGER48
		case "19": //UNSIGNED48
		case "0C": //TIME_OF_DAY
		case "0D": //TIME_DIFFERENCE
			return " 6";
		case "14": //INTEGER56
		case "1A": //UNSIGNED56
			return " 7";
		case "15": //INTEGER64
		case "1B": //UNSIGNED64
		case "11": //REAL64
			return " 8";
		case "09": //VISIBLE_STRING
			var str = value.length.toString();
			if(str.length==1) str = " " + str;
			return str;
		case "0A": //OCTET_STRING
			var str = (value.replace(/\s/g, "").length/2).toString();
			if(str.length==1) str = " " + str;
			return str;
		case "0B": //UNICODE_STRING
			var str = (value.replace(/\s/g, "").length/4).toString();
			if(str.length==1) str = " " + str;
			return str;
		case "0F": //DOMAIN
			return " 0";
		default:
			return "??";
	}
}

function dataType2pfFormat(dataType, value){
   var isHex = value.toLowerCase().search("x") >= 0;
	switch(dataType){
		case "02": return "I08";
		case "05": return isHex ? "X08" : "U08";
		case "03": return "I16";
		case "06": return isHex ? "X16" : "U16";
		case "10": return "I24";
		case "16": return isHex ? "X24" : "U24";
		case "04": return "I32";
		case "07": return isHex ? "X32" : "U32";
		case "08": return "R32";
		case "15": return "I64";
		case "1B": return isHex ? "X64" : "U64";
		case "11": return "R64";
		case "09": //VISIBLE_STRING
			var str = value.length.toString(16);
			if(str.length==1) str = "0" + str;
			return "S" + str;
		case "0A": //OCTET_STRING
			var str = (value.replace(/\s/g, "").length/2).toString(16);
			if(str.length==1) str = "0" + str;
			return "O" + str;
		case "0B": //UNICODE_STRING
			var str = (value.replace(/\s/g, "").length/4).toString(16);
			if(str.length==1) str = "0" + str;
			return "C" + str;
		case "0F": //DOMAIN
			return "Dxx";
		default:
			return "???";
	}
}

function value2c_code(value, dataType, object){
	dataType = dataType.slice(0, 2);
	value = value.replace("$NODEID+", "");
	var i = parseInt(value);
	switch(dataType){
		//case 0x01: return "01": //BOOLEAN;
		case "02": //INTEGER8;
			if(!(-128<=i && i<=127)){
				errorMessages.push("Error in object " + object + ": Invalid Default value!");
				return "0";
			}
			return i.toString();
		case "03": //INTEGER16;
			if(!(-32768<=i && i<=32767)){
				errorMessages.push("Error in object " + object + ": Invalid Default value!");
				return "0";
			}
			return i.toString();
		case "04": //INTEGER32;
			if(!(-2147483648<=i && i<=2147483647)){
				errorMessages.push("Error in object " + object + ": Invalid Default value!");
				return "0L";
			}
			return i.toString()+"L";
		case "05": //UNSIGNED8;
			if(!(0<=i && i<=255)){
				errorMessages.push("Error in object " + object + ": Invalid Default value!");
				return "0";
			}
			return "0x"+i.toString(16).toUpperCase();
		case "06": //UNSIGNED16;
			if(!(0<=i && i<=65536)){
				errorMessages.push("Error in object " + object + ": Invalid Default value!");
				return "0";
			}
			return "0x"+i.toString(16).toUpperCase();
		case "07": //UNSIGNED32;
			if(!(0<=i && i<=4294967295)){
				errorMessages.push("Error in object " + object + ": Invalid Default value!");
				return "0";
			}
			return "0x"+i.toString(16).toUpperCase()+"L";
		case "08": //REAL32;
      	var f = parseFloat(value);
			if(!(-3.4e38<f && f<3.4e38)){
				errorMessages.push("Error in object " + object + ": Invalid Default value!");
				return "0";
			}
			return f.toString();
		case "09": //VISIBLE_STRING;
			var strArray = value.match(/./g); //array of characters
			return "{'" + strArray.join("', '") + "'}";
		case "0A": //OCTET_STRING;
			//example for allowed formats: '11 AA BB 00 01' or '11AABB0001'
			var oct = value.replace(/[0-9A-F]{2}/gi, ""); //remove all pairs of two hex digits
			oct = oct.replace(/\s/g, ""); //remove all whitespaces
			if(oct.length){
				errorMessages.push("Error in object " + object + ": Invalid Default value!");
				return "0";
			}
			var strArray = value.match(/[0-9A-F]{2}/gi); //array of pairs
			return "{0x" + strArray.join(", 0x") + "}";
		default:
         return value;
			//errorMessages.push("Error in object " + object + ": Invalid Data Type!");
			//return "0";
	}
}

function attribute4c_code(memoryType, dataType, accessType, PDOmapping, TPDOdetectCOS){
	//for attribute description see generated CO_OD.h file
	var attr = 0;
	switch(memoryType){
		case "ROM": attr = 0x01; break;
		case "RAM": attr = 0x02; break;
		case "EEPROM": attr = 0x03; break;
	}
	if(accessType!="wo") attr |= 0x04;
	if(accessType=="wo" || accessType=="rw") attr |= 0x08;
	if(PDOmapping=="optional" || PDOmapping=="RPDO") attr |= 0x10;
	if(PDOmapping=="optional" || PDOmapping=="TPDO") attr |= 0x20;
	if(TPDOdetectCOS=="true") attr |= 0x40;
	if(!(dataType=="01" || dataType=="02" || dataType=="05" || dataType=="09" || dataType=="0A" || dataType=="0B" || dataType=="0F")){
		attr |= 0x80; //Multi_byte value
	}
	return attr;
}

function extractLabels(element){
	var output = <></>;

	//get languages
	var langs = [];
	for each(var lang in element.label.@lang)
		if(langs.indexOf(lang.toString()) < 0) langs.push(lang.toString());
	for each(var lang in element.description.@lang)
		if(langs.indexOf(lang.toString()) < 0) langs.push(lang.toString());
	langs.sort();

	for(var i=0; i<langs.length; i++){
		var lang = langs[i];
		var label = element.label.(@lang==lang).toString();
		var description = element.description.(@lang==lang);
		if(label || description.length()){
			var URI = description.@URI.toString();
			var more = "";
			if(URI) more = <a href={URI} target="infoWindow">More...</a>
			//replace XML illegar characters and replace all newlines in description with <br/>
			description = description.toString()
			if(description){
				description = description.replace(/&/g, "&amp;");
				description = description.replace(/</g, "&lt;");
				description = description.replace(/\n/g, "<br/>");
				description = new XML("<div>"+description+"</div>");
			}
			if(i>0) output += <br/>
			output +=
				<div class={"description lang_"+lang}>
					<h4>{lang}: {label}</h4>
					{description}
					{more}
				</div>;
		}
	}
	return output;
}

function generateCO_OD_H(){
	var text = "ODH"+
		"/*******************************************************************************\n"+
		"\n"+
		"   File: CO"+ODfileNameReference+"_OD.h\n"+
		"   CANopen Object Dictionary.\n"+
		"\n"+
		"   Copyright (C) 2004-2008 Janez Paternoster\n"+
		"\n"+
		"   License: GNU Lesser General Public License (LGPL).\n"+
		"\n"+
		"   <http://canopennode.sourceforge.net>\n"+
		"\n"+
		"   (For more information see <CO_SDO.h>.)\n"+
		"*/\n"+
		"/*\n"+
		"   This program is free software: you can redistribute it and/or modify\n"+
		"   it under the terms of the GNU Lesser General Public License as published by\n"+
		"   the Free Software Foundation, either version 3 of the License, or\n"+
		"   (at your option) any later version.\n"+
		"\n"+
		"   This program is distributed in the hope that it will be useful,\n"+
		"   but WITHOUT ANY WARRANTY; without even the implied warranty of\n"+
		"   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"+
		"   GNU Lesser General Public License for more details.\n"+
		"\n"+
		"   You should have received a copy of the GNU Lesser General Public License\n"+
		"   along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"+
		"\n"+
		"\n"+
		"   Author: Janez Paternoster\n"+
		"\n"+
		"\n"+
		"   This file was automatically generated with CANopenNode Object\n"+
		"   Dictionary Editor. DON'T EDIT THIS FILE MANUALLY !!!!\n"+
		"\n"+
		"*******************************************************************************/\n"+
		"\n"+
		"#ifndef _CO_OD"+ODfileNameReference+"_H\n"+
		"#define _CO_OD"+ODfileNameReference+"_H\n"+
		"\n"+
		"\n"+
		"/*******************************************************************************\n"+
		"   CANopen DATA DYPES\n"+
		"*******************************************************************************/\n"+
		"   typedef uint8_t      UNSIGNED8;\n"+
		"   typedef uint16_t     UNSIGNED16;\n"+
		"   typedef uint32_t     UNSIGNED32;\n"+
		"   typedef uint64_t     UNSIGNED64;\n"+
		"   typedef int8_t       INTEGER8;\n"+
		"   typedef int16_t      INTEGER16;\n"+
		"   typedef int32_t      INTEGER32;\n"+
		"   typedef int64_t      INTEGER64;\n"+
		"   typedef float32_t    REAL32;\n"+
		"   typedef float64_t    REAL64;\n"+
		"   typedef char_t       VISIBLE_STRING;\n"+
		"   typedef oChar_t      OCTET_STRING;\n"+
		"   typedef domain_t     DOMAIN;\n"+
		"\n"+
		"\n"+
		"/*******************************************************************************\n"+
		"   FILE INFO:\n"+
		"      FileName:     "+g_project.other.file.@fileName.toString()+"\n"+
		"      FileVersion:  "+g_project.other.file.@fileVersion.toString()+"\n"+
		"      CreationTime: "+g_project.other.file.@fileCreationTime.toString()+"\n"+
		"      CreationDate: "+g_project.other.file.@fileCreationDate.toString()+"\n"+
		"      CreatedBy:    "+g_project.other.file.@fileCreator.toString()+"\n"+
		"*******************************************************************************/\n"+
		"\n"+
		"\n"+
		"/*******************************************************************************\n"+
		"   DEVICE INFO:\n"+
		CO_OD_H_info.join("\n")+"\n"+
		"*******************************************************************************/\n"+
		"\n"+
		"\n"+
		"/*******************************************************************************\n"+
		"   FEATURES\n"+
		"*******************************************************************************/\n"+
		CO_OD_H_macros.join("\n")+"\n"+
		"\n"+
		"\n"+
		"/*******************************************************************************\n"+
		"   OBJECT DICTIONARY\n"+
		"*******************************************************************************/\n"+
		"   #define CO"+ODfileNameReference+"_OD_NoOfElements             "+CO_OD_C_OD.length.toString()+"\n"+
		"\n"+
		"\n"+
		"/*******************************************************************************\n"+
		"   TYPE DEFINITIONS FOR RECORDS\n"+
		"*******************************************************************************/\n"+
		CO_OD_H_typedefs.join("\n")+
		"\n"+
		"\n"+
		"/*******************************************************************************\n"+
		"   STRUCTURES FOR VARIABLES IN DIFFERENT MEMORY LOCATIONS\n"+
		"*******************************************************************************/\n"+
		"#define  CO"+ODfileNameReference+"_OD_FIRST_LAST_WORD     0x55 //Any value from 0x01 to 0xFE. If changed, EEPROM will be reinitialized.\n"+
		"\n"+
		"/***** Structure for RAM variables ********************************************/\n"+
		"struct sCO"+ODfileNameReference+"_OD_RAM{\n"+
		"               UNSIGNED32     FirstWord;\n"+
		"\n"+
		CO_OD_H_RAM.join("\n")+"\n"+
		"\n"+
		"               UNSIGNED32     LastWord;\n"+
		"};\n"+
		"\n"+
		"/***** Structure for EEPROM variables *****************************************/\n"+
		"struct sCO"+ODfileNameReference+"_OD_EEPROM{\n"+
		"               UNSIGNED32     FirstWord;\n"+
		"\n"+
		CO_OD_H_EEPROM.join("\n")+"\n"+
		"\n"+
		"               UNSIGNED32     LastWord;\n"+
		"};\n"+
		"\n"+
		"\n"+
		"/***** Structure for ROM variables ********************************************/\n"+
		"struct sCO"+ODfileNameReference+"_OD_ROM{\n"+
		"               UNSIGNED32     FirstWord;\n"+
		"\n"+
		CO_OD_H_ROM.join("\n")+"\n"+
		"\n"+
		"               UNSIGNED32     LastWord;\n"+
		"};\n"+
		"\n"+
		"\n"+
		"/***** Declaration of Object Dictionary variables *****************************/\n"+
		"extern struct sCO"+ODfileNameReference+"_OD_RAM CO"+ODfileNameReference+"_OD_RAM;\n"+
		"\n"+
		"extern struct sCO"+ODfileNameReference+"_OD_EEPROM CO"+ODfileNameReference+"_OD_EEPROM;\n"+
		"\n"+
		"extern struct sCO"+ODfileNameReference+"_OD_ROM CO"+ODfileNameReference+"_OD_ROM;\n"+
		"\n"+
		"\n"+
		"/*******************************************************************************\n"+
		"   ALIASES FOR OBJECT DICTIONARY VARIABLES\n"+
		"*******************************************************************************/\n"+
		CO_OD_H_aliases.join("\n")+"\n"+
		"\n"+
		"#endif\n";

		g_openerWindow.source.postMessage(text, "*");
}

function generateCO_OD_C(){
	var text = "ODC"+
		"/*******************************************************************************\n"+
		"\n"+
		"   File - CO"+ODfileNameReference+"_OD.c\n"+
		"   CANopen Object Dictionary.\n"+
		"\n"+
		"   Copyright (C) 2004-2008 Janez Paternoster\n"+
		"\n"+
		"   License: GNU Lesser General Public License (LGPL).\n"+
		"\n"+
		"   <http://canopennode.sourceforge.net>\n"+
		"\n"+
		"   (For more information see <CO_SDO.h>.)\n"+
		"*/\n"+
		"/*\n"+
		"   This program is free software: you can redistribute it and/or modify\n"+
		"   it under the terms of the GNU Lesser General Public License as published by\n"+
		"   the Free Software Foundation, either version 3 of the License, or\n"+
		"   (at your option) any later version.\n"+
		"\n"+
		"   This program is distributed in the hope that it will be useful,\n"+
		"   but WITHOUT ANY WARRANTY; without even the implied warranty of\n"+
		"   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"+
		"   GNU Lesser General Public License for more details.\n"+
		"\n"+
		"   You should have received a copy of the GNU Lesser General Public License\n"+
		"   along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"+
		"\n"+
		"\n"+
		"   Author: Janez Paternoster\n"+
		"\n"+
		"\n"+
		"   This file was automatically generated with CANopenNode Object\n"+
		"   Dictionary Editor. DON'T EDIT THIS FILE MANUALLY !!!!\n"+
		"\n"+
		"*******************************************************************************/\n"+
		"\n"+
		"\n"+
		"#include \"CO_driver.h\"\n"+
		"#include \"CO"+ODfileNameReference+"_OD.h\"\n"+
		"#include \"CO_SDO.h\"\n"+
		"\n"+
		"\n"+
		"/*******************************************************************************\n"+
		"   DEFINITION AND INITIALIZATION OF OBJECT DICTIONARY VARIABLES\n"+
		"*******************************************************************************/\n"+
		"\n"+
		"/***** Definition for RAM variables *******************************************/\n"+
		"struct sCO"+ODfileNameReference+"_OD_RAM CO"+ODfileNameReference+"_OD_RAM = {\n"+
		"           CO"+ODfileNameReference+"_OD_FIRST_LAST_WORD,\n"+
		"\n"+
		CO_OD_C_initRAM.join("\n")+"\n"+
		"\n"+
		"           CO"+ODfileNameReference+"_OD_FIRST_LAST_WORD,\n"+
		"};\n"+
		"\n"+
		"\n"+
		"/***** Definition for EEPROM variables ****************************************/\n"+
		"struct sCO"+ODfileNameReference+"_OD_EEPROM CO"+ODfileNameReference+"_OD_EEPROM = {\n"+
		"           CO"+ODfileNameReference+"_OD_FIRST_LAST_WORD,\n"+
		"\n"+
		CO_OD_C_initEEPROM.join("\n")+"\n"+
		"\n"+
		"           CO"+ODfileNameReference+"_OD_FIRST_LAST_WORD,\n"+
		"};\n"+
		"\n"+
		"\n"+
		"/***** Definition for ROM variables *******************************************/\n"+
		"   struct sCO"+ODfileNameReference+"_OD_ROM CO"+ODfileNameReference+"_OD_ROM = {    //constant variables, stored in flash\n"+
		"           CO"+ODfileNameReference+"_OD_FIRST_LAST_WORD,\n"+
		"\n"+
		CO_OD_C_initROM.join("\n")+"\n"+
		"\n"+
		"           CO"+ODfileNameReference+"_OD_FIRST_LAST_WORD\n"+
		"};\n"+
		"\n"+
		"\n"+
		"/*******************************************************************************\n"+
		"   STRUCTURES FOR RECORD TYPE OBJECTS\n"+
		"*******************************************************************************/\n"+
		CO_OD_C_records.join("\n")+"\n"+
		"\n"+
		"\n"+
		// "/*******************************************************************************\n"+
		// "   SDO SERVER ACCESS FUNCTIONS WITH USER CODE\n"+
		// "*******************************************************************************/\n"+
		// "#define WRITING (dir == 1)\n"+
		// "#define READING (dir == 0)\n"+
		// CO_OD_C_functions.join("\n")+
		// "\n"+
		// "\n"+
		"/*******************************************************************************\n"+
		"   OBJECT DICTIONARY\n"+
		"*******************************************************************************/\n"+
		"const CO_OD_entry_t CO"+ODfileNameReference+"_OD[CO"+ODfileNameReference+"_OD_NoOfElements] = {\n"+
		CO_OD_C_OD.join("\n")+"\n"+
		"};\n";

		g_openerWindow.source.postMessage(text, "*");
}

function generateEDSspec(){
	var num = EDSspec_optObj.length-1; EDSspec_optObj[0] = "SupportedObjects="+num.toString()+"\n"
	var num = EDSspec_manufObj.length-1; EDSspec_manufObj[0] = "SupportedObjects="+num.toString()+"\n"
	var text = "EDS"+
		"\n"+
		"; CANopen Electronic Data Sheet\n"+
		"; File was automatically generated by CANopenNode Object Dictionary Editor\n"+
		"; See http://canopennode.sourceforge.net/\n"+
		"\n"+
		"\n"+
		"[FileInfo]\n"+
		"FileName="+g_project.other.file.@fileName.toString()+"\n"+
		"FileVersion="+g_project.other.file.@fileVersion.toString()+"\n"+
		"FileRevision=0\n"+
		"EDSVersion=4.0\n"+
		"Description=Open Source CANopen implementation\n"+
		"CreationTime="+g_project.other.file.@fileCreationTime.toString()+"\n"+
		"CreationDate="+g_project.other.file.@fileCreationDate.toString()+"\n"+
		"CreatedBy="+g_project.other.file.@fileCreator.toString()+"\n"+
		"\n"+
		"\n"+
		"[DeviceInfo]\n"+
		EDSspec_info.join("")+
		"\n"+
		"\n"+
		"[Comments]\n"+
		"Lines=5\n"+
		"Line1=EDS File for CANopen device\n"+
		"Line2=Open Source CANopen implementation\n"+
		"Line3=Stack Version: V3.00\n"+
		"Line4=Generated by CANopenNode Object Dictionary Editor\n"+
		"line5=http://canopennode.sourceforge.net/\n"+
		"\n"+
		"\n"+
		"[DummyUsage]\n"+
		"Dummy0001=0\n"+
		"Dummy0002=1\n"+
		"Dummy0003=1\n"+
		"Dummy0004=1\n"+
		"Dummy0005=1\n"+
		"Dummy0006=1\n"+
		"Dummy0007=1\n"+
		"\n"+
		"\n"+
		"[MandatoryObjects]\n"+
		"SupportedObjects=3\n"+
		"1=0x1000\n"+
		"2=0x1001\n"+
		"3=0x1018\n"+
		"\n"+
		"\n"+
		"[OptionalObjects]\n"+
		EDSspec_optObj.join("")+
		"\n"+
		"\n"+
		"[ManufacturerObjects]\n"+
		EDSspec_manufObj.join("")+
		"\n"+
		EDSspec_objList.join("");

		g_openerWindow.source.postMessage(text, "*");
}

function generateXMLspec(){
	var file =
		<ISO15745ProfileContainer xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="CANopen_Main.xsd">
			<ISO15745Profile>
				<ProfileHeader>
					<ProfileIdentification>CAN device profile</ProfileIdentification>
					<ProfileRevision>1</ProfileRevision>
					<ProfileName>{g_project.other.DeviceIdentity.productName.toString()}</ProfileName>
					<ProfileSource/>
					<ProfileClassID>Device</ProfileClassID>
					<ISO15745Reference>
						<ISO15745Part>1</ISO15745Part>
						<ISO15745Edition>1</ISO15745Edition>
						<ProfileTechnology>CANopen</ProfileTechnology>
					</ISO15745Reference>
				</ProfileHeader>
				<ProfileBody xsi:type="ProfileBody_Device_CANopen" fileName={g_project.other.file.@fileName.toString()} fileCreator={g_project.other.file.@fileCreator.toString()} fileCreationDate={g_project.other.file.@fileCreationDate.toString()} fileCreationTime={g_project.other.file.@fileCreationTime.toString()} fileVersion={g_project.other.file.@fileVersion.toString()}>
					{g_project.other.DeviceIdentity}
					<DeviceFunction>
						{g_project.other.capabilities}
					</DeviceFunction>
				</ProfileBody>
			</ISO15745Profile>
			<ISO15745Profile>
				<ProfileHeader>
					<ProfileIdentification>CAN comm net profile</ProfileIdentification>
					<ProfileRevision>1</ProfileRevision>
					<ProfileName>{g_project.other.DeviceIdentity.productName.toString()}</ProfileName>
					<ProfileSource/>
					<ProfileClassID>CommunicationNetwork</ProfileClassID>
					<ISO15745Reference>
						<ISO15745Part>1</ISO15745Part>
						<ISO15745Edition>1</ISO15745Edition>
						<ProfileTechnology>CANopen</ProfileTechnology>
					</ISO15745Reference>
				</ProfileHeader>
				<ProfileBody xsi:type="ProfileBody_CommunicationNetwork_CANopen" fileName={g_project.other.file.@fileName.toString()} fileCreator={g_project.other.file.@fileCreator.toString()} fileCreationDate={g_project.other.file.@fileCreationDate.toString()} fileCreationTime={g_project.other.file.@fileCreationTime.toString()} fileVersion={g_project.other.file.@fileVersion.toString()}>
					<ApplicationLayers>
						<CANopenObjectList>
							{XMLspec_objList}
						</CANopenObjectList>
						{g_project.other.dummyUsage}
					</ApplicationLayers>
					<TransportLayers>
						<PhysicalLayer>
							{g_project.other.baudRate}
						</PhysicalLayer>
					</TransportLayers>
				</ProfileBody>
			</ISO15745Profile>
		</ISO15745ProfileContainer>

	g_openerWindow.source.postMessage("XDD<?xml version=\"1.0\"?>\n" + file.toXMLString(), "*");
}

function generateDOC(){
	var htmlBody =
		<body>
         <h3>CANopen object dictionary for {fileName}</h3>
         <ul id="ODList">
            <li class="expandable">
               <h2>Device information</h2>
               <ul>
                  <li class="expandable">
                     <div><h3>File information</h3></div>
                     <ul><table>
                        <tr>
                           <th>File Name</th>
                           <td>{g_project.other.file.@fileName.toString()}</td>
                        </tr>
                        <tr>
                           <th>File Version</th>
                           <td>{g_project.other.file.@fileVersion.toString()}</td>
                        </tr>
                        <tr>
                           <th>Creation Time</th>
                           <td>{g_project.other.file.@fileCreationTime.toString()}</td>
                        </tr>
                        <tr>
                           <th>Creation Date</th>
                           <td>{g_project.other.file.@fileCreationDate.toString()}</td>
                        </tr>
                        <tr>
                           <th>Created By</th>
                           <td>{g_project.other.file.@fileCreator.toString()}</td>
                        </tr>
                     </table></ul>
                  </li>
                  <li class="expandable">
                     <div><h3>Device Identity</h3></div>
                     <ul>{DOCidentity}</ul>
                  </li>
                  <li class="expandable">
                     <div><h3>CANopen features</h3></div>
                     <ul>{DOCfeatures}</ul>
                  </li>
               </ul>
            </li>
            <li class="expandable">
               <h2>Communication Profile</h2>
      			<ul class="PFContainer" style="display:block;">{DOCobjDictCommIndex}</ul>
            </li>
            <li class="expandable">
               <h2>Manufacturer specific</h2>
					<ul class="PFContainer">{DOCobjDictManufIndex}</ul>
            </li>
            <li class="expandable">
               <h2>Device Profile</h2>
					<ul class="PFContainer">{DOCobjDictProfileIndex}</ul>
            </li>
         </ul>
			<ul id="ODDoc">
            {DOCobjDictComm}
            {DOCobjDictManuf}
            {DOCobjDictProfile}
			</ul>
         <input type="button" value="Display everything" onclick="toggleAll();"/><br/>
			<footer>
				<em>Generated by <a href="http://canopennode.sourceforge.net/">CANopenNode</a>.</em>
			</footer>
		</body>;

	//HTML source
	var text = "HTM"+
		"<!DOCTYPE html>\n"+
		"<html><head>\n"+
		"  <meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>\n"+
		"  <title>"+fileName+" - CANopenNode</title>\n"+
		"  <link rel='stylesheet' type='text/css' href='lib/CANopenUI.css'/>\n"+
		"  <style type='text/css'>\n"+
		"     body{background-color: rgb(165, 186, 221);}\n"+
		"     li{list-style-type: none;}\n"+
		"     table, th, td{border: 1px solid black; padding: 0 5px; border-collapse: collapse;}\n"+
		"     #ODList{position: relative; width: 450px; margin: 0; padding: 0;}\n"+
		"     #ODList ul{padding-left: 30px;}\n"+
		"     #ODList > li{margin: 0 0 20px;}\n"+
		"     #ODList > li > *:first-child{font-family: sans-serif; background-color: LightBlue; margin: 0; padding: 5px 10px; cursor: pointer;}\n"+
		"     #ODList > li li.expandable{list-style-type: circle;}\n"+
		"     .expandable > *:first-child{cursor: pointer;}\n"+
		"     .expandable > *:nth-child(2){display: none;}\n"+   //rule[9], subject to toggleAll()
		"     #ODList input[addr]{position: absolute; left: 320px; width: 120px;}\n"+
		"     #ODList > li li.expandable input[addr]{left: 325px; width: 115px;}\n"+
		"     #ODList a{cursor: pointer; color: blue;}\n"+
		"     #ODDoc{position: fixed; overflow: auto; height: 95%; width: 500px; left: 500px; top: 10px;}\n"+   //rule[13], subject to toggleAll()
		"     #ODDoc > li{display: none;}\n"+   //rule[14], subject to toggleAll()
		"     .PFContainer .ODUpdatedAndDefault{background-color:lightGreen;}\n"+
		"     .PFContainer .ODUpdated{background-color:MediumSpringGreen;}\n"+
		"     .PFContainer .ODChanged{background-color:yellow;}\n"+
		"     .PFContainer .ODError{background-color:red;}\n"+
		"  </style>\n"+
		"  <script type='text/javascript' src='lib/CANopen.js'></script>\n"+
		"  <script type='text/javascript' src='lib/CANopenUI.js'></script>\n"+
		"  <script type='text/javascript'>\n"+
		"     //<![CDATA[\n"+
		"     var ODDocDisplayed = null;\n"+
		"     var ui = null;\n"+
		"     function toggle(){\n"+
		"        var tStyle = this.nextElementSibling.style;\n"+
		"        if(tStyle.display == 'block') tStyle.display = 'none';\n"+
		"        else tStyle.display = 'block';\n"+
		"     }\n"+
		"     function toggleAll(){\n"+
		"        if(document.styleSheets[1].cssRules[9].style.display == 'none'){\n"+
		"           document.styleSheets[1].cssRules[9].style.display = 'block';\n"+
		"           document.styleSheets[1].cssRules[14].style.display = 'block';\n"+
		"           document.styleSheets[1].cssRules[13].style.position = 'absolute';\n"+
		"           document.styleSheets[1].cssRules[13].style.overflow = 'visible';\n"+
		"        }\n"+
		"        else{\n"+
		"           document.styleSheets[1].cssRules[9].style.display = 'none';\n"+
		"           document.styleSheets[1].cssRules[14].style.display = 'none';\n"+
		"           document.styleSheets[1].cssRules[13].style.position = 'fixed';\n"+
		"           document.styleSheets[1].cssRules[13].style.overflow = 'auto';\n"+
		"        }\n"+
		"     }\n"+
		"     function showDoc(e){\n"+
		"        var docAttr = e.target.getAttribute('docref');\n"+
		"        if(docAttr){\n"+
		"           var docRef = document.getElementById(docAttr);\n"+
		"           if(docRef){\n"+
		"              if(ODDocDisplayed) ODDocDisplayed.style.display = 'none';\n"+
		"              ODDocDisplayed = docRef;\n"+
		"              docRef.style.display = 'block';\n"+
		"           }\n"+
		"        }\n"+
		"     }\n"+
		"     function init(){\n"+
		"        var expandables = document.getElementsByClassName('expandable');\n"+
		"        for(var i=0; i<expandables.length; i++){\n"+
		"           expandables[i].firstElementChild.addEventListener('click', toggle, false);\n"+
		"        }\n"+
		"        var ODList = document.getElementById('ODList');\n"+
		"        ODList.addEventListener('click', showDoc, false);\n"+
		"        //try{\n"+
  		"        ui = new CANopenUI();\n"+
		"        //}\n"+
		"        //catch(e){}\n"+
		"     }\n"+
		"     window.addEventListener('load', init, false);\n"+
		"     //]]>\n"+
		"  </script>\n"+
		"</head>\n"+
		htmlBody.toXMLString()+
		"</html>";

	g_openerWindow.source.postMessage(text, "*");
}
