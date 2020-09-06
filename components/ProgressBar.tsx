import React from "react";
import { View } from "react-native";
import { Text } from '../components/Themed';
export interface Props {
    bgcolor: string;
    completed: number;
}
const ProgressBar = (props: Props) => {
    const { bgcolor, completed } = props;
  
    const containerStyles = {
      height: 20,
      width: '100%',
      backgroundColor: "#e0e0de",
      borderRadius: 50
    }
  
    const fillerStyles = {
      height: '100%',
      width: `${completed}%`,
      backgroundColor: bgcolor,
      borderRadius: 5,
      textAlign: 'right'
    }
  
    const labelStyles = {
      padding: 5,
      color: 'white',
      fontWeight: 5
    }
  
    return (
      <View style={containerStyles}>
        <View style={fillerStyles}>
          <Text>{`${completed}%`}</Text>
        </View>
      </View>
    );
  };
  
  export default ProgressBar;
  