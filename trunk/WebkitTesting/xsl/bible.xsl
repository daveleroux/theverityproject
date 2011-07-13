<xsl:stylesheet
                  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                  version="2.0">
            <xsl:output  method="xml"
                        indent="yes"
                        omit-xml-declaration="yes"/>

	    <xsl:template match="/">				
			<xsl:apply-templates/>
	    </xsl:template>

	   <xsl:template match="note">
		<span class="note">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	   <xsl:template match="smallCaps">
		<span class="smallCaps">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	   <xsl:template match="b">
		<span class="b">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	   <xsl:template match="br">
		<br>
			<xsl:apply-templates/>
		</br>
	    </xsl:template>

	   <xsl:template match="i">
		<span class="i">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	   <xsl:template match="hebrew">
		<span class="hebrew">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	   <xsl:template match="paragraphTitle">
		<span class="paragraphTitle">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	   <xsl:template match="hebrewParagraph">
		<span class="paragraphTitle">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	   <xsl:template match="section">
		<span class="section">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	   <xsl:template match="psalmSuperscription">
		<span class="psalmSuperscription">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	    <xsl:template match="speakerHeading">
		<span class="speakerHeading">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	    <xsl:template match="bodyText">
		<span class="bodyText">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	    <xsl:template match="poetry">
		<span class="poetry">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	    <xsl:template match="bodyBlock">
		<span class="bodyBlock">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	    <xsl:template match="blockQuote">
		<span class="blockQuote">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	    <xsl:template match="chapter">
		<span class="chapter">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	    <xsl:template match="bookName">
		<span class="bookName">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	    <xsl:template match="verse">
		<span class="verse">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>

	    <xsl:template match="normalisedChapter">
		<span class="normalisedChapter">
			<xsl:apply-templates/>
		</span>
	    </xsl:template>


</xsl:stylesheet>
